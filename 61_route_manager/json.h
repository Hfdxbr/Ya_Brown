#pragma once

#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <variant>
#include <vector>

namespace Json {

class Node : public std::variant<std::vector<Node>, std::map<std::string, Node>,
                                 int, double, bool, std::string> {
 public:
  using variant::variant;

  // Non constant methods
  auto& AsArray() { return std::get<std::vector<Node>>(*this); }
  auto& AsMap() { return std::get<std::map<std::string, Node>>(*this); }
  int& AsInt() { return std::get<int>(*this); }
  double& AsDouble() { return std::get<double>(*this); }
  bool& AsBoolean() { return std::get<bool>(*this); }
  auto& AsString() { return std::get<std::string>(*this); }
  // Constant methods
  const auto& AsArray() const { return std::get<std::vector<Node>>(*this); }
  const auto& AsMap() const {
    return std::get<std::map<std::string, Node>>(*this);
  }
  int AsInt() const { return std::get<int>(*this); }
  // int AsInt() const { return IsInt() ? std::get<int>(*this) : AsBoolean(); }
  double AsDouble() const {
    if (IsDouble())
      return std::get<double>(*this);
    else
      return AsInt();
  }
  bool AsBoolean() const { return std::get<bool>(*this); }
  const auto& AsString() const { return std::get<std::string>(*this); }
  // Type getters
  bool IsArray() const {
    return std::holds_alternative<std::vector<Node>>(*this);
  }
  bool IsMap() const {
    return std::holds_alternative<std::map<std::string, Node>>(*this);
  }
  bool IsInt() const { return std::holds_alternative<int>(*this); }
  bool IsDouble() const { return std::holds_alternative<double>(*this); }
  bool IsBoolean() const { return std::holds_alternative<bool>(*this); }
  bool IsString() const { return std::holds_alternative<std::string>(*this); }

  std::string ToString() const {
    std::stringstream ss;
    if (IsArray()) {
      ss << '[';
      for (auto& x : AsArray()) ss << x.ToString() << ',';
      auto out = ss.str();
      if (out == "[")
        out.push_back(']');
      else
        out.back() = ']';
      return out;
    } else if (IsMap()) {
      ss << '{';
      for (const auto& [key, value] : AsMap())
        ss << "\"" << key << "\":" << value.ToString() << ',';
      auto out = ss.str();
      if (out == "{")
        out.push_back('}');
      else
        out.back() = '}';
      return out;
    } else if (IsDouble()) {
      ss << AsDouble();
      return ss.str();
    } else if (IsInt()) {
      ss << AsInt();
      return ss.str();
    } else if (IsString()) {
      ss << '"' << AsString() << '"';
      return ss.str();
    } else if (IsBoolean())
      return AsBoolean() ? "true" : "false";
    else
      return "";
  }
};

class Document {
 public:
  explicit Document(Node root);

  const Node& GetRoot() const;

 private:
  Node root;
};

Document Load(std::istream& input);
}  // namespace Json

std::ostream& operator<<(std::ostream& os, const Json::Document& doc);