#include "ini.h"

#include <string_view>

pair<string_view, string_view> Split(string_view line, char by) {
  size_t pos = line.find(by);
  string_view left = line.substr(0, pos);

  if (pos < line.size() && pos + 1 < line.size()) {
    return {left, line.substr(pos + 1)};
  } else {
    return {left, string_view()};
  }
}

string_view Unbrace(string_view value) {
  if (!value.empty() && value.front() == '[') {
    value.remove_prefix(1);
  }
  if (!value.empty() && value.back() == ']') {
    value.remove_suffix(1);
  }
  return value;
}

namespace Ini {
  Section& Document::AddSection(string name) { return sections[name]; }
  const Section& Document::GetSection(const string& name) const { return sections.at(name); }
  size_t Document::SectionCount() const { return sections.size(); }



  string GetSectionName(string_view line) {
    return string(Unbrace(line));
  }

  pair<string, string> GetSectionItem(string_view line) {
      auto [key, value] = Split(line, '=');
      return { string(key), string(value) };
  }


  Document Load(istream& input) {
    Document doc;
    Section* section = nullptr;
    string line;
    while(getline(input, line)) {
      if(line.front() == '[' && line.back() == ']') {
        section = &doc.AddSection(GetSectionName(line));
        continue;
      }
      if(section && line.find('=') != string::npos)
        section->insert(GetSectionItem(line));
    }
    return doc;
  }
}