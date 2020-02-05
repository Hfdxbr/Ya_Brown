#include "PrintStats.h"

#include <algorithm>
#include <iostream>
#include <vector>

void PrintStats(std::vector<Person> persons) {
  auto females_end =
      partition(persons.begin(), persons.end(),
                [](const Person& p) { return p.gender == Gender::FEMALE; });
  auto females_begin = persons.begin();
  auto employed_females_end =
      partition(females_begin, females_end,
                [](const Person& p) { return p.is_employed; });
  auto males_begin = females_end;
  auto males_end = persons.end();
  auto employed_males_end = partition(
      males_begin, males_end, [](const Person& p) { return p.is_employed; });

  char nl = '\n';
  std::cout << "Median age = "
            << ComputeMedianAge(persons.begin(), persons.end()) << nl
            << "Median age for females = "
            << ComputeMedianAge(females_begin, females_end) << nl
            << "Median age for males = "
            << ComputeMedianAge(males_begin, males_end) << nl
            << "Median age for employed females = "
            << ComputeMedianAge(females_begin, employed_females_end) << nl
            << "Median age for unemployed females = "
            << ComputeMedianAge(employed_females_end, females_end) << nl
            << "Median age for employed males = "
            << ComputeMedianAge(males_begin, employed_males_end) << nl
            << "Median age for unemployed males = "
            << ComputeMedianAge(employed_males_end, males_end) << nl;
}