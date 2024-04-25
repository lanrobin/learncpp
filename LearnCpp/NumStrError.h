#pragma once

#include "StudyCaseRegister.h"

class num_str_error : public i_study_case {
public:
  std::string name() override;
  void test() override;
};