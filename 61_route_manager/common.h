#pragma once

#include <cmath>
#include <optional>
#include <sstream>
#include <string>
#include <string_view>

std::pair<std::string_view, std::optional<std::string_view>> SplitTwoStrict(
    std::string_view s, std::string_view delimiter = " ") {
  const size_t pos = s.find(delimiter);
  if (pos == s.npos) {
    return {s, std::nullopt};
  } else {
    return {s.substr(0, pos), s.substr(pos + delimiter.length())};
  }
}

std::pair<std::string_view, std::string_view> SplitTwo(
    std::string_view s, std::string_view delimiter = " ") {
  const auto [lhs, rhs_opt] = SplitTwoStrict(s, delimiter);
  return {lhs, rhs_opt.value_or("")};
}

std::string_view ReadToken(std::string_view& s,
                           std::string_view delimiter = " ") {
  const auto [lhs, rhs] = SplitTwo(s, delimiter);
  s = rhs;
  return lhs;
}

std::string_view RemoveSpaces(std::string_view s) {
  if (s.empty()) return s;
  while (s.front() == ' ') s.remove_prefix(1);
  while (s.back() == ' ') s.remove_suffix(1);
  return s;
}

template <class T>
T Convert(std::string_view str);

template <>
int Convert<int>(std::string_view str) {
  size_t pos;
  const int result = std::stoi(std::string(str), &pos);
  if (pos != str.length()) {
    std::stringstream error;
    error << "string " << str << " contains " << (str.length() - pos)
          << " trailing chars";
    throw std::invalid_argument(error.str());
  }
  return result;
}

template <>
double Convert<double>(std::string_view str) {
  size_t pos;
  const double result = std::stod(std::string(str), &pos);
  if (pos != str.length()) {
    std::stringstream error;
    error << "string " << str << " contains " << (str.length() - pos)
          << " trailing chars";
    throw std::invalid_argument(error.str());
  }
  return result;
}

struct Coords {
  double latitude = 0;
  double longitude = 0;
};

namespace Constants {
double pi_deg = 180;
double pi = 3.1415926535;
int R = 6'371'000;  // meters
}  // namespace Constants

double DegreeToRad(double angle) {
  return angle * Constants::pi / Constants::pi_deg;
}

double CalculateGeoLength(Coords v1, Coords v2) {
  auto dl = DegreeToRad(std::abs(v1.longitude - v2.longitude));
  auto f1 = DegreeToRad(v1.latitude);
  auto f2 = DegreeToRad(v2.latitude);

  double num1 = cos(f2) * sin(dl);
  double num2 = cos(f1) * sin(f2) - sin(f1) * cos(f2) * cos(dl);
  double denom = sin(f1) * sin(f2) + cos(f1) * cos(f2) * cos(dl);

  double angle = atan(sqrt(num1 * num1 + num2 * num2) / denom);
  return angle * Constants::R;
}