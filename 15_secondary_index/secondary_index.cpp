#include "../test_runner.h"

#include <iostream>
#include <map>
#include <string>
#include <unordered_map>

using namespace std;

struct Record {
  string id;
  string title;
  string user;
  int timestamp;
  int karma;
};

// Реализуйте этот класс
class Database {
private:
  using um_iterator = typename unordered_map<string, Record>::iterator;
  unordered_map<string, Record> data_by_id;
  multimap<int, um_iterator> data_by_timestamp;
  multimap<int, um_iterator> data_by_karma;
  multimap<string, um_iterator> data_by_user;
  template<class T> using mm_iterator = typename multimap<T, um_iterator>::iterator;
  unordered_map<string, mm_iterator<int>> timestamp_by_id;
  unordered_map<string, mm_iterator<int>> karma_by_id;
  unordered_map<string, mm_iterator<string>> user_by_id;


public:
  bool Put(const Record& record) {
    auto result = data_by_id.insert({record.id, record});
    if(result.second){
      timestamp_by_id[record.id] = data_by_timestamp.insert({record.timestamp, result.first});
      karma_by_id[record.id] = data_by_karma.insert({record.karma, result.first});
      user_by_id[record.id] = data_by_user.insert({record.user, result.first});
      return true;
    }
    return false;
  }

  const Record* GetById(const string& id) const {
    auto it = data_by_id.find(id);
    if(it != data_by_id.end()) {
      return &it->second;
    }
    return nullptr;
  }

  bool Erase(const string& id) {
    auto data_it = data_by_id.find(id);
    if(data_it != data_by_id.end()) {
      data_by_id.erase(data_it);
      data_by_timestamp.erase(timestamp_by_id[id]);
      data_by_karma.erase(karma_by_id[id]);
      data_by_user.erase(user_by_id[id]);
      return true;
    }
    return false;
  }

  template <typename Callback>
  void RangeByTimestamp(int low, int high, Callback callback) const {
    auto low_it = data_by_timestamp.lower_bound(low);
    auto upp_it = data_by_timestamp.upper_bound(high);
    for(auto it = low_it; it != upp_it; ++it)
      if(!callback(it->second->second))
        return;
  }

  template <typename Callback>
  void RangeByKarma(int low, int high, Callback callback) const {
    auto low_it = data_by_karma.lower_bound(low);
    auto upp_it = data_by_karma.upper_bound(high);
    for(auto it = low_it; it != upp_it; ++it)
      if(!callback(it->second->second))
        return;
  }

  template <typename Callback>
  void AllByUser(const string& user, Callback callback) const {
    auto low_it = data_by_user.lower_bound(user);
    auto upp_it = data_by_user.upper_bound(user);
    for(auto it = low_it; it != upp_it; ++it)
      if(!callback(it->second->second))
        return;
  }
};

void TestRangeBoundaries() {
  const int good_karma = 1000;
  const int bad_karma = -10;

  Database db;
  db.Put({"id1", "Hello there", "master", 1536107260, good_karma});
  db.Put({"id2", "O>>-<", "general2", 1536107260, bad_karma});

  int count = 0;
  db.RangeByKarma(bad_karma, good_karma, [&count](const Record&) {
    ++count;
    return true;
  });

  ASSERT_EQUAL(2, count);
}

void TestSameUser() {
  Database db;
  db.Put({"id1", "Don't sell", "master", 1536107260, 1000});
  db.Put({"id2", "Rethink life", "master", 1536107260, 2000});

  int count = 0;
  db.AllByUser("master", [&count](const Record&) {
    ++count;
    return true;
  });

  ASSERT_EQUAL(2, count);
}

void TestReplacement() {
  const string final_body = "Feeling sad";

  Database db;
  db.Put({"id", "Have a hand", "not-master", 1536107260, 10});
  db.Erase("id");
  db.Put({"id", final_body, "not-master", 1536107260, -10});

  auto record = db.GetById("id");
  ASSERT(record != nullptr);
  ASSERT_EQUAL(final_body, record->title);
}

int main() {
  TestRunner tr;
  RUN_TEST(tr, TestRangeBoundaries);
  RUN_TEST(tr, TestSameUser);
  RUN_TEST(tr, TestReplacement);
  return 0;
}
