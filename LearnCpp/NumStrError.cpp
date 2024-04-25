#include <string>
#include <charconv>
#include <iostream>
#include "NumStrError.h"

std::string num_str_error::name() { return std::string("num_str_error"); }

void num_str_error::test() {
  const int n = 123456;

  // Can use any container, string, array, etc.
  std::string str;
  str.resize(6); // hold enough storage for each digit of `n`

  const auto [ptr, ec] = std::to_chars(str.data(), str.data() + str.size(), n);

  if (ec == std::errc{}) {
    std::cout << "str:" << str << ",ptr"<<ptr << std::endl;
  }      // 123
  else { /* handle failure */
    std::cout << std::make_error_code(ec).message() << '\n';
  }
}