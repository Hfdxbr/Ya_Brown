#include <future>
#include <list>
#include <unordered_map>

#include "Common.h"

using namespace std;

class LruCache : public ICache {
 public:
  LruCache(shared_ptr<IBooksUnpacker> books_unpacker, const Settings& settings)
      : unpacker(books_unpacker), settings(settings) {
    used_memory = 0;
  }

  BookPtr GetBook(const string& book_name) override {
    lock_guard<mutex> lg(m);
    if (auto it = book_by_name.find(book_name); it != book_by_name.end()) {
      ordered_by_request.remove(it->second);
      ordered_by_request.push_back(it->second);
      return it->second;
    }

    auto ptr = BookPtr(unpacker->UnpackBook(book_name));
    size_t size = ptr->GetContent().size();

    if (size > settings.max_memory) {
      ordered_by_request.clear();
      book_by_name.clear();
      used_memory = 0;
      return ptr;
    }

    while (used_memory + size > settings.max_memory) {
      auto last_ptr = ordered_by_request.front();
      ordered_by_request.pop_front();
      book_by_name.erase(last_ptr->GetName());
      used_memory -= last_ptr->GetContent().size();
    }

    book_by_name[book_name] = ptr;
    ordered_by_request.push_back(ptr);
    used_memory += size;

    return ptr;
  }

 private:
  shared_ptr<IBooksUnpacker> unpacker;
  Settings settings;
  list<BookPtr> ordered_by_request;
  unordered_map<string, BookPtr> book_by_name;
  size_t used_memory;
  mutex m;
};

unique_ptr<ICache> MakeCache(shared_ptr<IBooksUnpacker> books_unpacker,
                             const ICache::Settings& settings) {
  return make_unique<LruCache>(move(books_unpacker), settings);
}
