#pragma once

#include <string>
#include <vector>
#include <memory>
#include <type_traits>

class i_study_case {
public:
  virtual std::string name() = 0;
  virtual void test() = 0;

  template <typename T> 
  static void register_study_case();
  static void invoke_all_testcases();
};



