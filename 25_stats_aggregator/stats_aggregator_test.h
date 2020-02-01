#pragma once

#include "stats_aggregator.h"
#include "../test_runner.h"

#include <sstream>

namespace StatsAggregators{
  void TestSum();
  void TestMin();
  void TestMax();
  void TestAverage();
  void TestMode();
  void TestComposite();
}