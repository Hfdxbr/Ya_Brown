#pragma once

#include "common.h"
#include "json.h"

#include <set>
#include <unordered_map>

class StopManager {
 private:
  struct Stop {
    Coords pos;
    std::set<std::string> buses;
    std::unordered_map<std::string, int> distances;
  };

  using Stops = std::unordered_map<std::string, Stop>;
  Stops stops_;

  void Load(const std::string& stop_name, Coords coords,
            std::unordered_map<std::string, int>& distances);

 public:
  struct ProcessResult {
    bool success;
    const std::string& stop_name;
    const std::set<std::string>& buses;
  };
  void Load(std::string_view input);
  void Load(const Json::Node& input);
  ProcessResult Process(const std::string& stop_name) const;

  const Stops& Get() const { return stops_; }
  void Update(const std::string& bus_id, const std::string& stop_name);
};

void StopManager::Update(const std::string& bus_id,
                         const std::string& stop_name) {
  auto stop_it = stops_.insert({stop_name, Stop()});
  stop_it.first->second.buses.insert(bus_id);
}

void StopManager::Load(const std::string& stop_name, Coords coords,
                       std::unordered_map<std::string, int>& distances) {
  if (stop_name.empty()) {
    std::stringstream error;
    error << "Wrong data: Stop name = " << stop_name
          << "; latitude = " << coords.latitude
          << "; longitude = " << coords.longitude;
    throw std::invalid_argument(error.str());
  }

  auto& stop = stops_[stop_name];
  stop.pos = coords;
  stop.distances = std::move(distances);
}

void StopManager::Load(std::string_view input) {
  auto stop_name = std::string(RemoveSpaces(ReadToken(input, ":")));
  auto latitude = Convert<double>(RemoveSpaces(ReadToken(input, ",")));
  auto longitude = Convert<double>(RemoveSpaces(ReadToken(input, ",")));

  std::unordered_map<std::string, int> distances;
  while (!input.empty()) {
    auto target_distance = RemoveSpaces(ReadToken(input, ","));
    auto dist = RemoveSpaces(ReadToken(target_distance, "to"));
    auto target = std::string(RemoveSpaces(target_distance));
    distances[target] = Convert<int>(RemoveSpaces(ReadToken(dist, "m")));
  }
  Load(stop_name, {latitude, longitude}, distances);
}

void StopManager::Load(const Json::Node& input) {
  auto& input_map = input.AsMap();
  auto stop_name = input_map.at("name").AsString();
  auto latitude = input_map.at("latitude").AsDouble();
  auto longitude = input_map.at("longitude").AsDouble();

  std::unordered_map<std::string, int> distances;
  if (input_map.find("road_distances") != input_map.end())
    for (const auto& [key, value] : input_map.at("road_distances").AsMap())
      distances[key] = value.AsInt();

  Load(stop_name, {latitude, longitude}, distances);
}

StopManager::ProcessResult StopManager::Process(
    const std::string& stop_name) const {
  auto stop_it = stops_.find(stop_name);
  if (stop_it == stops_.end())
    return {.success = false, .stop_name = stop_name, .buses = {}};

  return {.success = true, .stop_name = stop_name, stop_it->second.buses};
}

template <class T>
T Convert(const StopManager::ProcessResult& res);

template <>
std::string Convert<std::string>(const StopManager::ProcessResult& res) {
  std::stringstream ss;
  if (res.success) {
    if (res.buses.empty())
      ss << "Stop " << res.stop_name << ": no buses";
    else {
      ss << "Stop " << res.stop_name << ": buses";
      for (auto& bus : res.buses) ss << " " << bus;
    }
  } else
    ss << "Stop " << res.stop_name << ": not found";
  return ss.str();
}

template <>
Json::Node Convert<Json::Node>(const StopManager::ProcessResult& res) {
  Json::Node root{std::map<std::string, Json::Node>()};
  if (res.success) {
    root.AsMap()["buses"] = Json::Node(std::vector<Json::Node>());
    auto& buses = root.AsMap()["buses"].AsArray();
    buses.reserve(res.buses.size());
    for (auto& bus : res.buses) buses.push_back(Json::Node(bus));
  } else {
    root.AsMap()["error_message"] = Json::Node(std::string("not found"));
  }
  return root;
}