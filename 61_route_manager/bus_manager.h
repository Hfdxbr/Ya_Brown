#pragma once

#include "common.h"
#include "json.h"
#include "stop_manager.h"

#include <functional>
#include <iomanip>
#include <set>
#include <unordered_map>

class BusManager {
 private:
  struct Bus {
    bool is_route_looped;
    std::vector<std::string> stops;
  };
  using Buses = std::unordered_map<std::string, Bus>;
  Buses buses_;
  StopManager& sm_;

  void Load(const std::string& bus_id, bool looped,
            std::vector<std::string>& stops);

 public:
  struct ProcessResult {
    bool success;
    const std::string& bus_id;
    int stops_n;
    int stops_n_unique;
    int road_len;
    double geo_len;
  };
  BusManager(StopManager& sm) : sm_(sm) {}
  void Load(std::string_view input);
  void Load(const Json::Node& input);
  ProcessResult Process(const std::string& bus_id);
};

void BusManager::Load(const std::string& bus_id, bool looped,
                      std::vector<std::string>& stops) {
  auto bus_it = buses_.find(bus_id);
  if (bus_it != buses_.end()) {
    std::stringstream error;
    error << "Redefinition: Bus id = " << bus_id << " is already defined";
    throw std::invalid_argument(error.str());
  }

  auto& bus = buses_[bus_id];
  bus.is_route_looped = looped;
  bus.stops = std::move(stops);
  for (auto& stop : bus.stops) sm_.Update(bus_id, stop);
}

void BusManager::Load(std::string_view input) {
  auto bus_id = std::string(RemoveSpaces(ReadToken(input, ":")));
  std::pair<int, int> delim_counter;
  for (auto& it : input) {
    if (it == '-')
      ++delim_counter.first;
    else if (it == '>')
      ++delim_counter.second;
  }

  std::string_view delim = delim_counter.first == 0 ? ">" : "-";
  std::vector<std::string> stops;
  stops.reserve(delim_counter.first + delim_counter.second);
  while (!input.empty())
    stops.push_back(std::string(RemoveSpaces(ReadToken(input, delim))));

  Load(bus_id, delim_counter.first == 0, stops);
}

void BusManager::Load(const Json::Node& input) {
  auto& input_map = input.AsMap();
  auto bus_id = input_map.at("name").AsString();

  std::vector<std::string> stops;
  if (input_map.find("stops") != input_map.end()) {
    auto& stops_array = input_map.at("stops").AsArray();
    stops.reserve(stops_array.size());
    for (auto& stop : stops_array) stops.push_back(stop.AsString());
  }
  bool is_looped = input_map.at("is_roundtrip").AsBoolean();
  Load(bus_id, is_looped, stops);
}

template <class Output, class Iterator, class Func>
Output AccumulateWithNext(Iterator begin, Iterator end, Func f) {
  Output result = Output();
  for (auto s2 = begin, s1 = s2++; s2 != end; ++s1, ++s2) result += f(*s1, *s2);

  return result;
}

BusManager::ProcessResult BusManager::Process(const std::string& bus_id) {
  auto bus_it = buses_.find(bus_id);
  if (bus_it == buses_.end()) return {.success = false, .bus_id = bus_id};

  auto& bus = buses_[bus_id];
  bool is_looped = buses_[bus_id].is_route_looped;
  int n = is_looped ? bus.stops.size() : 2 * bus.stops.size() - 1;
  int n_unique =
      std::set<std::string>(bus.stops.begin(), bus.stops.end()).size();

  double len_geo = AccumulateWithNext<double>(
      bus.stops.begin(), bus.stops.end(), [&](auto& x, auto& y) {
        return CalculateGeoLength(sm_.Get().at(x).pos, sm_.Get().at(y).pos);
      });
  if (!is_looped) len_geo *= 2;

  auto GetRoadDistance = [&](std::string& x, std::string& y) {
    auto& stop1 = sm_.Get().at(x);
    auto& stop2 = sm_.Get().at(y);
    if (stop1.distances.find(y) == stop1.distances.end())
      return stop2.distances.at(x);
    else
      return stop1.distances.at(y);
  };

  int len_road = AccumulateWithNext<int>(bus.stops.begin(), bus.stops.end(),
                                         GetRoadDistance);
  if (!is_looped)
    len_road += AccumulateWithNext<int>(bus.stops.rbegin(), bus.stops.rend(),
                                        GetRoadDistance);

  return {true, bus_id, n, n_unique, len_road, len_geo};
}

template <class T>
T Convert(const BusManager::ProcessResult& res);

template <>
std::string Convert<std::string>(const BusManager::ProcessResult& res) {
  std::stringstream ss;
  if (res.success)
    ss << "Bus " << res.bus_id << ": " << res.stops_n << " stops on route, "
       << res.stops_n_unique << " unique stops, " << res.road_len
       << " route length, " << std::setprecision(7)
       << res.road_len * 1.0 / res.geo_len << " curvature";
  else
    ss << "Bus " << res.bus_id << ": not found";
  return ss.str();
}

template <>
Json::Node Convert<Json::Node>(const BusManager::ProcessResult& res) {
  Json::Node root{std::map<std::string, Json::Node>()};
  auto& output = root.AsMap();
  if (res.success) {
    output["route_length"] = Json::Node(res.road_len);
    output["curvature"] = Json::Node(res.road_len * 1.0 / res.geo_len);
    output["stop_count"] = Json::Node(res.stops_n);
    output["unique_stop_count"] = Json::Node(res.stops_n_unique);
  } else {
    output["error_message"] = Json::Node(std::string("not found"));
  }

  return root;
}