#include "json.h"
#include <sstream>

using namespace std;

std::variant<int, double> ReadNumber(std::istream& is) {
  int integer;
  double rational;
  is >> integer;
  if (is.peek() == '.') {
    is >> rational;
    return rational + integer;
  } else {
    return integer;
  }
}

std::ostream& operator<<(std::ostream& os, const Json::Document& doc) {
  auto str = doc.GetRoot().ToString();
  os << str;
  return os;
}

namespace Json {

Document::Document(Node root) : root(move(root)) {}

const Node& Document::GetRoot() const { return root; }

Node LoadNode(istream& input);

Node LoadArray(istream& input) {
  vector<Node> result;

  for (char c; input >> c && c != ']';) {
    if (c != ',') {
      input.putback(c);
    }
    result.push_back(LoadNode(input));
  }

  return Node(move(result));
}

Node LoadNumber(istream& input) {
  auto result = ReadNumber(input);
  if (std::holds_alternative<int>(result))
    return Node(std::get<int>(result));
  else
    return Node(std::get<double>(result));
}

Node LoadBoolean(istream& input) {
  bool result;
  input >> std::boolalpha >> result;
  return Node(result);
}

Node LoadString(istream& input) {
  string line;
  getline(input, line, '"');
  return Node(move(line));
}

Node LoadDict(istream& input) {
  map<string, Node> result;

  for (char c; input >> c && c != '}';) {
    if (c == ',') {
      input >> c;
    }

    string key = LoadString(input).AsString();
    input >> c;
    result.emplace(move(key), LoadNode(input));
  }

  return Node(move(result));
}

Node LoadNode(istream& input) {
  char c;
  input >> c;

  if (c == '[') {
    return LoadArray(input);
  } else if (c == '{') {
    return LoadDict(input);
  } else if (c == '"') {
    return LoadString(input);
  } else if (std::isdigit(c) || c == '-') {
    input.putback(c);
    return LoadNumber(input);
  } else {
    input.putback(c);
    return LoadBoolean(input);
  }
}

Document Load(istream& input) { return Document{LoadNode(input)}; }

}  // namespace Json
