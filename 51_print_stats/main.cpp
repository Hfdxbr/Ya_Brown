#include <vector>

#include "Person.h"
#include "PrintStats.h"

int main() {
  std::vector<Person> persons = {
      {31, Gender::MALE, false},   {40, Gender::FEMALE, true},
      {24, Gender::MALE, true},    {20, Gender::FEMALE, true},
      {80, Gender::FEMALE, false}, {78, Gender::MALE, false},
      {10, Gender::FEMALE, false}, {55, Gender::MALE, true},
  };
  PrintStats(persons);
  return 0;
}