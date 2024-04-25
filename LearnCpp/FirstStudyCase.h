#pragma once

#include "StudyCaseRegister.h"

class first_study_case :public i_study_case
{
public:
  first_study_case() {}
  std::string name() override;
  void test() override;
};