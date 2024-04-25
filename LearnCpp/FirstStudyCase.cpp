#include "StudyCaseRegister.h"
#include "FirstStudyCase.h"
#include <iostream>

std::string first_study_case::name() {
  return std::string("first study case.");
}

void first_study_case::test() {
  std::cout << "start to invoke:" << name() << std::endl;

  std::cout << "finished to invoke:" << name() << std::endl;
}