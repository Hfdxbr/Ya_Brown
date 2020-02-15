#include "bus_manager.h"
#include "json.h"
#include "stop_manager.h"

#include <cmath>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <memory>
#include <optional>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

class RouteManager {
 private:
  StopManager sm_;
  BusManager bm_;

 public:
  RouteManager() : bm_(sm_) {}
  RouteManager(std::istream& is) : bm_(sm_) { Load(is); }
  RouteManager(const Json::Document& idoc) : bm_(sm_) { Load(idoc); }
  void Load(std::istream& is);
  void Load(const Json::Document& idoc);
  std::vector<std::string> Process(std::istream& is);
  Json::Document Process(const Json::Document& idoc);
};

void RouteManager::Load(std::istream& is) {
  int n;
  is >> n;
  std::string request_type, data;
  while (n--) {
    is >> request_type;
    std::getline(is, data);
    if (request_type == "Bus") {
      bm_.Load(std::string_view(data));
    } else if (request_type == "Stop") {
      sm_.Load(std::string_view(data));
    } else {
    }
  }
}

void RouteManager::Load(const Json::Document& idoc) {
  auto& input_map = idoc.GetRoot().AsMap();
  if (input_map.find("base_requests") == input_map.end()) return;
  const auto& requests = input_map.at("base_requests").AsArray();
  for (auto& req : requests) {
    auto& request_type = req.AsMap().at("type").AsString();
    if (request_type == "Bus") {
      bm_.Load(req);
    } else if (request_type == "Stop") {
      sm_.Load(req);
    } else {
    }
  }
}

std::vector<std::string> RouteManager::Process(std::istream& is) {
  int n;
  is >> n;
  std::string request_type, data;
  std::vector<std::string> output;

  while (n--) {
    is >> request_type;
    std::getline(is, data);
    if (request_type == "Bus") {
      output.push_back(Convert<std::string>(bm_.Process(data)));
    } else if (request_type == "Stop") {
      output.push_back(Convert<std::string>(sm_.Process(data)));
    } else {
    }
  }

  return output;
}

Json::Document RouteManager::Process(const Json::Document& idoc) {
  std::vector<Json::Node> output;
  auto& input_map = idoc.GetRoot().AsMap();
  if (input_map.find("stat_requests") == input_map.end())
    return Json::Document({});
  const auto& requests = input_map.at("stat_requests").AsArray();
  for (auto& req : requests) {
    auto& request_type = req.AsMap().at("type").AsString();
    if (request_type == "Bus") {
      auto res =
          Convert<Json::Node>(bm_.Process(req.AsMap().at("name").AsString()));
      res.AsMap()["request_id"] = req.AsMap().at("id");
      output.push_back(res);
    } else if (request_type == "Stop") {
      auto res =
          Convert<Json::Node>(sm_.Process(req.AsMap().at("name").AsString()));
      res.AsMap()["request_id"] = req.AsMap().at("id");
      output.push_back(res);
    } else {
    }
  }
  return Json::Document(Json::Node(std::move(output)));
}

int main() {
  // RouteManager rt(std::cin);
  // for (auto& x : rt.ProcessRaw(std::cin)) std::cout << x << std::endl;

  auto doc = Json::Load(std::cin);
  RouteManager rt;

  rt.Load(doc);
  auto root = rt.Process(doc);
  std::cout << root << std::endl;

  return 0;
}