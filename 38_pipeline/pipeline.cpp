#include "../test_runner.h"
#include <functional>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

using namespace std;


struct Email {
  string from;
  string to;
  string body;
};

class Worker {
 public:
  virtual ~Worker() = default;
  virtual void Process(unique_ptr<Email> email) = 0;
  virtual void Run() {
    // только первому worker-у в пайплайне нужно это имплементировать
    throw logic_error("Unimplemented");
  }

 protected:
  // реализации должны вызывать PassOn, чтобы передать объект дальше
  // по цепочке обработчиков
  void PassOn(unique_ptr<Email> email) const {
    if (next_worker) next_worker->Process(move(email));
  }
  unique_ptr<Worker> next_worker;

 public:
  void SetNext(unique_ptr<Worker> next) {
    if (!next) return;
    next_worker = move(next);
  }
};

class Reader : public Worker {
 public:
  Reader(istream& is) : is(is) {}

  void Run() override {
    string from;
    while (getline(is, from)) {
      Email email{from,"",""};
      getline(is, email.to);
      getline(is, email.body);
      PassOn(make_unique<Email>(email));
    }
  }

  void Process(unique_ptr<Email> email) override {};
 private:
  istream& is;
};

class Filter : public Worker {
 public:
  using Function = function<bool(const Email&)>;

 public:
  Filter(Function filter) : filter(move(filter)) {}

  void Process(unique_ptr<Email> email) override {
    if (email && filter(*email)) PassOn(move(email));
  }

 private:
  Function filter;
};

class Copier : public Worker {
 public:
  Copier(string to) : to(move(to)) {}

  void Process(unique_ptr<Email> email) override {
    if (!email) return;
    if (email->to != to) {
      auto copy = make_unique<Email>(Email{email->from, to, email->body});
      PassOn(move(email));
      PassOn(move(copy));
    } else
      PassOn(move(email));
  }

 private:
  string to;
};

class Sender : public Worker {
 public:
  Sender(ostream& os) : os(os) {}

  void Process(unique_ptr<Email> email) override {
    if (!email) return;
    char nl = '\n';
    os << email->from << nl << email->to << nl << email->body << nl;
    PassOn(move(email));
  }

private:
  ostream& os;
};

// реализуйте класс
class PipelineBuilder {
public:
  // добавляет в качестве первого обработчика Reader
  explicit PipelineBuilder(istream& in) : workers_chain(make_unique<Reader>(in)), last_worker(workers_chain.get()) {}

  // добавляет новый обработчик Filter
  PipelineBuilder& FilterBy(Filter::Function filter) {
    InsertWorker(make_unique<Filter>(move(filter)));
    return *this;
  }

  // добавляет новый обработчик Copier
  PipelineBuilder& CopyTo(string recipient) {
    InsertWorker(make_unique<Copier>(move(recipient)));
    return *this;
  }

  // добавляет новый обработчик Sender
  PipelineBuilder& Send(ostream& out) {
    InsertWorker(make_unique<Sender>(out));
    return *this;
  }

  // возвращает готовую цепочку обработчиков
  unique_ptr<Worker> Build() {
    return move(workers_chain);
  }
private:
  void InsertWorker(unique_ptr<Worker> new_worker) {
    auto new_last_worker = new_worker.get();
    last_worker->SetNext(move(new_worker));
    last_worker = new_last_worker;
  }
  unique_ptr<Worker> workers_chain;
  Worker* last_worker;
};


void TestSanity() {
  string input = (
    "erich@example.com\n"
    "richard@example.com\n"
    "Hello there\n"

    "erich@example.com\n"
    "ralph@example.com\n"
    "Are you sure you pressed the right button?\n"

    "ralph@example.com\n"
    "erich@example.com\n"
    "I do not make mistakes of that kind\n"
  );
  istringstream inStream(input);
  ostringstream outStream;

  PipelineBuilder builder(inStream);
  builder.FilterBy([](const Email& email) {
    return email.from == "erich@example.com";
  });
  builder.CopyTo("richard@example.com");
  builder.Send(outStream);
  auto pipeline = builder.Build();

  pipeline->Run();

  string expectedOutput = (
    "erich@example.com\n"
    "richard@example.com\n"
    "Hello there\n"

    "erich@example.com\n"
    "ralph@example.com\n"
    "Are you sure you pressed the right button?\n"

    "erich@example.com\n"
    "richard@example.com\n"
    "Are you sure you pressed the right button?\n"
  );

  ASSERT_EQUAL(expectedOutput, outStream.str());
}

int main() {
  TestRunner tr;
  RUN_TEST(tr, TestSanity);
  return 0;
}
