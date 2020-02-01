#include "../test_runner.h"
#include <algorithm>
#include <iostream>
#include <iterator>
#include <memory>
#include <set>
#include <utility>
#include <vector>

using namespace std;

template <typename T>
class PriorityCollection {
public:
  using Id = unsigned int/* тип, используемый для идентификаторов */;
  int DeletedItemRank = -1;

  // Добавить объект с нулевым приоритетом
  // с помощью перемещения и вернуть его идентификатор
  Id Add(T object) {
    Id max_id = data.size();
    data.push_back({0, move(object)});
    id_by_rank.insert({0, max_id});
    return max_id;
  }

  // Добавить все элементы диапазона [range_begin, range_end)
  // с помощью перемещения, записав выданные им идентификаторы
  // в диапазон [ids_begin, ...)
  template <typename ObjInputIt, typename IdOutputIt>
  void Add(ObjInputIt range_begin, ObjInputIt range_end,
           IdOutputIt ids_begin) {
    while(range_begin != range_end) {
      *ids_begin++ = Add(move(*range_begin++));
    }
  }

  // Определить, принадлежит ли идентификатор какому-либо
  // хранящемуся в контейнере объекту
  bool IsValid(Id id) const {
    return id < data.size() && data[id].rank != DeletedItemRank;
  }

  // Получить объект по идентификатору
  const T& Get(Id id) const {
    return data.at(id).obj;
  }

  // Увеличить приоритет объекта на 1
  void Promote(Id id) {
    if(IsValid(id)){
      id_by_rank.erase({data[id].rank, id});
      id_by_rank.insert({++data[id].rank, id});
    }
  }

  // Получить объект с максимальным приоритетом и его приоритет
  pair<const T&, int> GetMax() const {
    int max_rank_id = id_by_rank.rbegin()->second;
    const Obj& item = data[max_rank_id];
    return {item.obj, item.rank};
  }

  // Аналогично GetMax, но удаляет элемент из контейнера
  pair<T, int> PopMax() {
    int max_rank_id = id_by_rank.rbegin()->second;
    Obj& item = data[max_rank_id];
    id_by_rank.erase({item.rank, max_rank_id});
    int rank = item.rank;
    item.rank = DeletedItemRank;
    return {move(item.obj), rank};
  }

private:
  // Приватные поля и методы
  struct Obj {
    int rank;
    T obj;
  };

  set<pair<int, Id>> id_by_rank;
  vector<Obj> data;
};


class StringNonCopyable : public string {
public:
  using string::string;  // Позволяет использовать конструкторы строки
  StringNonCopyable(const StringNonCopyable&) = delete;
  StringNonCopyable(StringNonCopyable&&) = default;
  StringNonCopyable& operator=(const StringNonCopyable&) = delete;
  StringNonCopyable& operator=(StringNonCopyable&&) = default;
};

void TestNoCopy() {
  PriorityCollection<StringNonCopyable> strings;
  const auto white_id = strings.Add("white");
  const auto yellow_id = strings.Add("yellow");
  const auto red_id = strings.Add("red");

  strings.Promote(yellow_id);
  for (int i = 0; i < 2; ++i) {
    strings.Promote(red_id);
  }
  strings.Promote(yellow_id);
  {
    const auto item = strings.PopMax();
    ASSERT_EQUAL(item.first, "red");
    ASSERT_EQUAL(item.second, 2);
  }
  {
    const auto item = strings.PopMax();
    ASSERT_EQUAL(item.first, "yellow");
    ASSERT_EQUAL(item.second, 2);
  }
  {
    const auto item = strings.PopMax();
    ASSERT_EQUAL(item.first, "white");
    ASSERT_EQUAL(item.second, 0);
  }
}

int main() {
  TestRunner tr;
  RUN_TEST(tr, TestNoCopy);
  return 0;
}
