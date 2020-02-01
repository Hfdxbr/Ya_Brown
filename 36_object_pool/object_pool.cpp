#include "../test_runner.h"

#include <algorithm>
#include <iostream>
#include <string>
#include <queue>
#include <stdexcept>
#include <set>
using namespace std;

template <class T>
class ObjectPool {
 public:
  T* Allocate() {
    T* item;
    if (freed.empty()) {
      allocated.insert(item = new T());
    } else {
      allocated.insert(item = freed.front());
      freed.pop();
    }
    return item;
  }

  T* TryAllocate() {
    T* item;
    if (freed.empty()) {
      item = nullptr;
    } else {
      allocated.insert(item = freed.front());
      freed.pop();
    }
    return item;
  }

  void Deallocate(T* object) {
    auto it = allocated.find(object);
    if (it == allocated.end())
      throw invalid_argument("Unknown object");
    else {
      freed.push(*it);
      allocated.erase(it);
    }
  }

  ~ObjectPool() {
    for (auto& ptr : allocated) delete ptr;
    while (!freed.empty()) {
      delete freed.front();
      freed.pop();
    }
  }

 private:
  set<T*> allocated;
  queue<T*> freed;
};

void TestObjectPool() {
  ObjectPool<string> pool;

  auto p1 = pool.Allocate();
  auto p2 = pool.Allocate();
  auto p3 = pool.Allocate();

  *p1 = "first";
  *p2 = "second";
  *p3 = "third";

  pool.Deallocate(p2);
  ASSERT_EQUAL(*pool.Allocate(), "second");

  pool.Deallocate(p3);
  pool.Deallocate(p1);
  ASSERT_EQUAL(*pool.Allocate(), "third");
  ASSERT_EQUAL(*pool.Allocate(), "first");

  pool.Deallocate(p1);
}

int main() {
  TestRunner tr;
  RUN_TEST(tr, TestObjectPool);
  return 0;
}
