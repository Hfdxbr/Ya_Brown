#include <algorithm>
#include <iostream>
#include <numeric>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

using namespace std;

template <typename Iterator>
class IteratorRange {
 public:
  IteratorRange(Iterator begin, Iterator end) : first(begin), last(end) {}

  Iterator begin() const { return first; }

  Iterator end() const { return last; }

 private:
  Iterator first, last;
};

template <typename Collection>
auto Head(Collection& v, size_t top) {
  return IteratorRange{v.begin(), next(v.begin(), min(top, v.size()))};
}

struct Person {
  string name;
  int age, income;
  bool is_male;
};

template <typename Iterator>
string GetTopName(IteratorRange<Iterator> range) {
  if (range.begin() == range.end()) {
    return string();
  } else {
    sort(range.begin(), range.end(), [](const Person& lhs, const Person& rhs) {
      return lhs.name < rhs.name;
    });
    const string* most_popular_name = &range.begin()->name;
    int count = 1;
    for (auto i = range.begin(); i != range.end();) {
      auto same_name_end = find_if_not(
          i, range.end(), [i](const Person& p) { return p.name == i->name; });
      auto cur_name_count = std::distance(i, same_name_end);
      if (cur_name_count > count) {
        count = cur_name_count;
        most_popular_name = &i->name;
      }
      i = same_name_end;
    }
    return *most_popular_name;
  }
}

class PeopleStats {
 private:
  vector<Person> people_by_age;
  vector<int> accumulated_incomes;
  string top_name_M;
  string top_name_W;

 public:
  PeopleStats(istream& input) {
    int count;
    input >> count;

    people_by_age = vector<Person>(count);
    unordered_map<string, int> namesM;
    unordered_map<string, int> namesW;
    accumulated_incomes.reserve(count);
    int j = count;
    for (int i = 0; i < j;) {
      char gender;
      Person p;
      input >> p.name >> p.age >> p.income >> gender;
      p.is_male = gender == 'M';
      if (p.is_male)
        people_by_age[i++] = p;
      else
        people_by_age[--j] = p;

      accumulated_incomes.push_back(p.income);
    }

    top_name_M = GetTopName(
        IteratorRange{people_by_age.begin(), people_by_age.begin() + j});
    top_name_W = GetTopName(
        IteratorRange{people_by_age.begin() + j, people_by_age.end()});

    sort(begin(accumulated_incomes), end(accumulated_incomes),
         [](const int& lhs, const int& rhs) { return lhs > rhs; });

    for (int i = 1; i < count; ++i)
      accumulated_incomes[i] += accumulated_incomes[i - 1];

    sort(begin(people_by_age), end(people_by_age),
         [](const Person& lhs, const Person& rhs) { return lhs.age < rhs.age; });
  }
  size_t CountOlder(int age) const {
    auto adult_begin =
        lower_bound(begin(people_by_age), end(people_by_age), age,
                    [](const Person& lhs, int age) { return lhs.age < age; });
    return distance(adult_begin, end(people_by_age));
  }

  int SumIncomeOfTop(size_t n) const {
    return n == 0 ? 0 : accumulated_incomes[n - 1];
  }

  string GetTopNameW() const { return top_name_W; }
  string GetTopNameM() const { return top_name_M; }
};

int main() {
  const PeopleStats people(cin);
  for (string command; cin >> command;) {
    if (command == "AGE") {
      int adult_age;
      cin >> adult_age;
      cout << "There are " << people.CountOlder(adult_age)
           << " adult people for maturity age " << adult_age << '\n';
    } else if (command == "WEALTHY") {
      int count;
      cin >> count;

      cout << "Top-" << count << " people have total income "
           << people.SumIncomeOfTop(count) << '\n';
    } else if (command == "POPULAR_NAME") {
      char gender;
      cin >> gender;
      string name;
      if (gender == 'M') name = people.GetTopNameM();
      if (gender == 'W') name = people.GetTopNameW();
      if (name.empty()) {
        cout << "No people of gender " << gender << '\n';
      } else {
        cout << "Most popular name among people of gender " << gender << " is "
             << name << '\n';
      }
    }
  }
}
