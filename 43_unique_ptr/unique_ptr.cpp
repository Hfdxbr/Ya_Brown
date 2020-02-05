#include "../test_runner.h"

#include <cstddef>  // нужно для nullptr_t

using namespace std;

// Реализуйте шаблон класса UniquePtr
template <typename T>
class UniquePtr {
 private:
  T* data;

 public:
  UniquePtr() : data(nullptr) {}
  UniquePtr(T* ptr) : data(ptr) {}
  UniquePtr(const UniquePtr&) = delete;
  UniquePtr(UniquePtr&& other) : data(other.data) { other.data = nullptr; }
  UniquePtr& operator=(const UniquePtr&) = delete;

  UniquePtr& operator=(nullptr_t) {
    delete data;
    data = nullptr;

    return *this;
  }

  UniquePtr& operator=(UniquePtr&& other) {
    if (this != &other) {
      delete data;
      data = other.data;
      other.data = nullptr;
    }
    return *this;
  }
  ~UniquePtr() { delete data; }

  T& operator*() const { return *data; }

  T* operator->() const { return data; }

  T* Release() {
    T* output = data;
    data = nullptr;
    return output;
  }

  void Reset(T* ptr) {
    delete data;
    data = ptr;
  }

  void Swap(UniquePtr& other) {
    T* temp = data;
    data = other.data;
    other.data = temp;
  }

  T* Get() const { return data; }
};

struct Item {
  static int counter;
  int value;
  Item(int v = 0): value(v) {
    ++counter;
  }
  Item(const Item& other): value(other.value) {
    ++counter;
  }
  ~Item() {
    --counter;
  }
};

int Item::counter = 0;


void TestLifetime() {
  Item::counter = 0;
  {
    UniquePtr<Item> ptr(new Item);
    ASSERT_EQUAL(Item::counter, 1);

    ptr.Reset(new Item);
    ASSERT_EQUAL(Item::counter, 1);
  }
  ASSERT_EQUAL(Item::counter, 0);

  {
    UniquePtr<Item> ptr(new Item);
    ASSERT_EQUAL(Item::counter, 1);

    auto rawPtr = ptr.Release();
    ASSERT_EQUAL(Item::counter, 1);

    delete rawPtr;
    ASSERT_EQUAL(Item::counter, 0);
  }
  ASSERT_EQUAL(Item::counter, 0);
}

void TestGetters() {
  UniquePtr<Item> ptr(new Item(42));
  ASSERT_EQUAL(ptr.Get()->value, 42);
  ASSERT_EQUAL((*ptr).value, 42);
  ASSERT_EQUAL(ptr->value, 42);
}

int main() {
  TestRunner tr;
  RUN_TEST(tr, TestLifetime);
  RUN_TEST(tr, TestGetters);
}
