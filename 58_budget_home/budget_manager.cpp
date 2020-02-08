#include <ctime>
#include <iostream>
#include <map>
#include <numeric>
#include <string>
#include <string_view>
#include <tuple>
#include <vector>

using namespace std;

enum Month {
  BEGIN = 1,
  JAN = 1,
  FEB,
  MAR,
  APR,
  MAY,
  JUN,
  JUL,
  AUG,
  SEP,
  OCT,
  NOV,
  DEC,
  END
};

Month operator++(Month &m) {
  m = static_cast<Month>(m + 1);
  return m;
}

vector<string_view> SplitByChar(string_view s, char delim) {
  vector<string_view> output;
  size_t first = 0;

  while (first < s.size()) {
    const auto second = s.find(delim, first);

    if (first != second)
      output.emplace_back(s.substr(first, second - first));

    if (second == string_view::npos)
      break;

    first = second + 1;
  }

  return output;
}

int NumOfDays(Month m, int year = 2020) {
  if (m == Month::FEB)
    return (year - 2020) % 4 == 0 ? 29 : 28;
  int number = static_cast<int>(m);
  if (number > 7)
    number -= 7;
  return number % 2 == 0 ? 30 : 31;
}

struct Date {
  int d;
  Month m;
  int y;
  Date(string_view yyyymmdd) {
    auto date = SplitByChar(yyyymmdd, '-');
    if (date.size() != 3)
      throw "bad date format";
    y = stoi(string(date[0]));
    m = static_cast<Month>(stoi(string(date[1])));
    if (m < Month::BEGIN || m >= Month::END)
      throw "invalid month";
    d = stoi(string(date[2]));
    if (d < 1 || d > NumOfDays(m, y))
      throw "invalid day";
  }

  Date &operator++() {
    ++d;
    if (d > NumOfDays(m, y)) {
      d = 1;
      ++m;
      if (m == Month::END) {
        m = Month::BEGIN;
        ++y;
      }
    }
    return *this;
  }

  time_t AsTimestamp() const {
    tm t;
    t.tm_sec = 0;
    t.tm_min = 0;
    t.tm_hour = 0;
    t.tm_mday = d;
    t.tm_mon = m - 1;
    t.tm_year = y - 1900;
    t.tm_isdst = 0;
    return mktime(&t);
  }
};

#define DECL_DATE_COMP(op)                                                     \
  bool operator op(const Date &a, const Date &b) {                             \
    return tie(a.y, a.m, a.d) op tie(b.y, b.m, b.d);                           \
  }

DECL_DATE_COMP(<);
DECL_DATE_COMP(<=);
DECL_DATE_COMP(==);

int ComputeDaysDiff(const Date &date_to, const Date &date_from) {
  const time_t timestamp_to = date_to.AsTimestamp();
  const time_t timestamp_from = date_from.AsTimestamp();
  static const int SECONDS_IN_DAY = 60 * 60 * 24;
  return (timestamp_to - timestamp_from) / SECONDS_IN_DAY;
}

class BudgetManager {
public:
  BudgetManager(double tax_rate = 0.13) : untax_rate(1. - tax_rate) {}
  void SetTaxRate(double tax_rate) { this->untax_rate = tax_rate; }

  double ComputeIncome(const Date &from, const Date &to) const {
    auto from_ = earn_by_date.lower_bound(from);
    auto to_ = earn_by_date.upper_bound(to);

    return accumulate(from_, to_, 0.0,
                      [](double acc, auto &item) { return acc + item.second; });
  }

  void Earn(const Date &from, const Date &to, double value) {
    double value_per_day = value / (ComputeDaysDiff(to, from) + 1);
    for (Date date = from; date <= to; ++date)
      earn_by_date[date] += value_per_day;
  }

  void PayTax(const Date &from, const Date &to) {
    for (Date date = from; date <= to; ++date)
      earn_by_date[date] *= untax_rate;
  }

private:
  map<Date, double> earn_by_date;
  double untax_rate;
};

int main() {
  int count;
  cin >> count;
  BudgetManager budget;
  for (int i = 0; i < count; ++i) {
    string request, from, to;
    cin >> request >> from >> to;
    if (request == "Earn") {
      int value;
      cin >> value;
      budget.Earn(Date(from), Date(to), value);
    }
    if (request == "PayTax") {
      budget.PayTax(Date(from), Date(to));
    }
    if (request == "ComputeIncome") {
      cout << fixed << budget.ComputeIncome(Date(from), Date(to)) << endl;
    }
  }
  return 0;
}