// LearnCpp.cpp : Defines the entry point for the application.
//

#include "LearnCpp.h"

#include "FirstStudyCase.h"
#include "StudyCaseRegister.h"
#include "NumStrError.h"
using namespace std;

int main() {
  cout << "Hello CMake." << endl;
  auto c = std::make_shared<first_study_case>();
  c->test();

  std::make_shared<num_str_error>()->test();
  std::cout << "Press any key to exit." << std::endl;
  getchar();
  return 0;
}
