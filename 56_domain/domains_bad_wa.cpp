#include <algorithm>
#include <iostream>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

using namespace std;

vector<string_view> SplitByChar(string_view s, char delim) {
  std::vector<std::string_view> output;
  size_t first = 0;

  while (first < s.size()) {
    const auto second = s.find(delim, first);

    if (first != second)
      output.emplace_back(s.substr(first, second - first));

    if (second == std::string_view::npos)
      break;

    first = second + 1;
  }

  return output;
}

class DomainFilter {
  struct DomainDict {
    unordered_map<string, shared_ptr<DomainDict>> data;
    DomainDict &operator[](string_view s) {
      if (Has(s))
        return *data[string(s)];
      else
        return *(data[string(s)] = make_shared<DomainDict>());
    }
    bool Has(string_view s) const { return data.find(string(s)) != data.end(); }
    bool Final() const { return data.find(".") != data.end(); }
  };

public:
  DomainFilter(const vector<string> &banned_domains) {
    for (const auto &domain : banned_domains) {
      DomainDict *curr = &dd;
      auto splitted = SplitByChar(domain, '.');
      for (auto it = splitted.rbegin(); it != splitted.rend(); ++it)
        if (!curr->Final())
          curr = &((*curr)[string(*it)]);
        else
          break;
      curr->data = {{".", nullptr}};
    }
  }

  bool Has(string_view s) {
    auto splitted = SplitByChar(s, '.');
    DomainDict *curr = &dd;
    for (auto it = splitted.rbegin(); it != splitted.rend(); ++it) {
      if (curr->Has(string(*it)))
        curr = &((*curr)[string(*it)]);
      else if (curr->Final())
        return true;
      else
        return false;
    }
    return curr->Final();
  }

private:
  DomainDict dd;
};

vector<string> ReadDomains() {
  size_t count;
  string tmp;
  getline(cin, tmp);
  count = stoi(tmp);
  vector<string> domains;
  for (size_t i = 0; i < count; ++i) {
    string domain;
    getline(cin, domain);
    domains.push_back(domain);
  }
  return domains;
}

int main() {
  DomainFilter banned_domains(ReadDomains());
  const vector<string> domains_to_check = ReadDomains();

  for (const string_view domain : domains_to_check) {
    if (!banned_domains.Has(domain)) {
      cout << "Good" << endl;
    } else {
      cout << "Bad" << endl;
    }
  }
  return 0;
}
