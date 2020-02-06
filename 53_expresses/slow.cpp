#include <algorithm>
#include <cmath>
#include <iostream>
#include <unordered_map>
#include <set>
#include <string>
#include <vector>

using namespace std;

class RouteManager {
public:
  void AddRoute(int start, int finish) {
    reachable_lists_[start].insert(finish);
    reachable_lists_[finish].insert(start);
  }
  int FindNearestFinish(int start, int finish) const {
    int result = abs(start - finish);
    auto it_s = reachable_lists_.find(start);
    if (it_s == reachable_lists_.end() || result == 0) {
        return result;
    }
    auto& reachable_stations = it_s->second;
    if (!reachable_stations.empty()) {
      if(finish >= *reachable_stations.rbegin())
        return min(result, finish - *reachable_stations.rbegin());
      if(finish <= *reachable_stations.begin())
        return min(result, *reachable_stations.begin() - finish);
      auto right = reachable_stations.lower_bound(finish);
      auto left = prev(right);
      return min(result, min(finish - *left, *right - finish));
    }
    return result;
  }
private:
  unordered_map<int, set<int>> reachable_lists_;
};


int main() {
  RouteManager routes;

  int query_count;
  cin >> query_count;

  for (int query_id = 0; query_id < query_count; ++query_id) {
    string query_type;
    cin >> query_type;
    int start, finish;
    cin >> start >> finish;
    if (query_type == "ADD") {
      routes.AddRoute(start, finish);
    } else if (query_type == "GO") {
      cout << routes.FindNearestFinish(start, finish) << "\n";
    }
  }

  return 0;
}
