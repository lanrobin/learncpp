#include <vector>
#include <memory>
#include "StudyCaseRegister.h"

static std::vector<std::shared_ptr<i_study_case>> test_cases;

template <typename T> 
void i_study_case::register_study_case() {
  static_assert(std::is_base_of_v<i_study_case, T>,
                "T must be derived from i_study_case");
  test_cases.push_back(std::make_shared(T))
}


void i_study_case::invoke_all_testcases() {
  for (const auto &ptr : test_cases) {
    ptr->test();
  }
}