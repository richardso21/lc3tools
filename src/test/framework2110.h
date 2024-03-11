/*
 * Copyright 2020 McGraw-Hill Education. All rights reserved. No reproduction or
 * distribution without the prior written consent of McGraw-Hill Education.
 */
#include <cstdint>
#include <functional>
#include <sstream>
#include <string>
#include <unordered_set>
#include <vector>

#include "framework_common.h"

namespace framework2110 {
class Tester;

using test_func_t = std::function<void(lc3::sim &, Tester &)>;

extern std::function<void(Tester &)> setup;
extern std::function<void(void)> shutdown;
extern std::function<void(lc3::sim &)> testBringup;
extern std::function<void(lc3::sim &)> testTeardown;

struct TestCase {
  std::string name;
  test_func_t test_func;
  int randomizeSeed;

  TestCase(std::string const &name, test_func_t test_func, int randomizeSeed);
};

struct TestPart {
  std::string label;
  std::string message;
};

struct TestResult {
  std::string test_name;
  std::vector<TestPart *> parts;
  std::unordered_set<int> fail_inds;
  std::string output;
  TestPart *error;
  double seed;
};

class Tester {
 private:
  std::vector<TestCase> tests;
  bool print_output, ignore_privilege, verbose;
  uint32_t print_level;
  uint64_t seed;
  std::vector<std::string> obj_filenames;
  lc3::core::SymbolTable symbol_table;

  BufferedPrinter *printer;
  StringInputter *inputter;
  lc3::sim *simulator;

  std::vector<TestResult> test_results;
  TestResult curr_test_result;
  std::ostringstream curr_output;

  void testAll(void);
  void testSingle(std::string const &test_name);

  void testSingle(TestCase const &test);

  void appendTestPart(std::string const &label, std::string const &message,
                      bool pass);

  void printResults();

  void printJson();

  double checkSimilarityHelper(std::vector<char> const &source,
                               std::vector<char> const &target) const;

  friend int main(int argc, char *argv[]);

 public:
  Tester(bool print_output, uint32_t print_level, bool ignore_privilege,
         bool verbose, uint64_t seed,
         std::vector<std::string> const &obj_filenames);

  void registerTest(std::string const &name, test_func_t test_func,
                    int randomizeSeed);

  template <typename T>
  void verify(std::string const &label, T out, T expected, bool (*comp)(T, T),
              std::string (*print)(T)) {
    std::ostringstream message;
    message << "Expected: " << print(expected) << ", Got: " << print(out)
            << std::endl;
    appendTestPart(label, message.str(), comp(out, expected));
  };
  template <typename T>
  void verify(std::string const &label, T out, T expected) {
    std::ostringstream message;
    message << "Expected: " << expected << ", Got: " << out << std::endl;
    appendTestPart(label, message.str(), out == expected);
  };

  void verify(std::string const &label, bool pass) {
    appendTestPart(label, "", pass);
  };

  void output(std::string const &message);
  void debugOutput(std::string const &message);
  void error(std::string const &label, std::string const &message);

  void setInputString(std::string const &source) {
    inputter->setString(source);
  }
  void setInputCharDelay(uint32_t inst_count) {
    inputter->setCharDelay(inst_count);
  }

  std::string getConsoleOutput(void) const;
  void clearConsoleOutput(void) { printer->clear(); }
  bool checkMatch(std::string const &a, std::string const &b) const {
    return a == b;
  }

  lc3::core::SymbolTable const &getSymbolTable(void) const {
    return symbol_table;
  }

  uint16_t get_symbol_location(const std::string &symbol);

  void write_mem_at_symbol(const std::string &symbol, std::uint16_t val);
  std::uint16_t read_mem_at_symbol(const std::string &symbol);
  void write_string_at_symbol(const std::string &symbol,
                              const std::string &str);

  std::string read_mem_string(std::uint16_t addr);
  std::string read_mem_string(std::uint16_t addr, std::size_t len);

 private:
  void setSymbolTable(lc3::core::SymbolTable const &symbol_table) {
    this->symbol_table = symbol_table;
  }
};

class Tester_error final {
  std::string lbl, msg;

 public:
  Tester_error(std::string l, std::string m) : lbl{l}, msg{m} {}
  void report(Tester &tester) const noexcept { tester.error(lbl, msg); }
};

class Quoted {
  const std::string &str;

 public:
  Quoted(const std::string &s) : str{s} {}

  friend inline std::ostream &operator<<(std::ostream &os, Quoted q) {
    os << '"';
    for (char c : q.str)
      if (c == '\n')
        os << "\\n";
      else if (c < ' ' || c >= 127) {
        auto flags = os.flags();
        // print c in octal
        os << '\\' << std::noshowbase << std::oct
           << int{static_cast<unsigned char>(c)};
        os.flags(flags);
      } else
        os << c;
    os << '"';
    return os;
  }
};

// Allows creation of strings from character arrays with embedded \0s.
template <std::size_t N>
inline std::string make_string(const char (&arr)[N]) {
  return {arr, N - 1};
}

int main(int argc, char *argv[]);
};  // namespace framework2110
