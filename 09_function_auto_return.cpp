#include <iostream>
#include <vector>

using namespace std;

template<typename T>
auto func(T t) { return t; }

#if defined(ERROR_EXAMPLE)
auto func_return_difference_type(bool flag) {
   if (flag) return 1;
   else return 2L; // error
}
// inconsistent deduction for auto return type: ‘int’ and then ‘long’

auto func_return_initializer_list() {
   return {1, 2, 3}; // error returning initializer list
}

#endif


std::vector<int> func_return_initializer_list() {
   return {1, 2, 3};
}

int main() {
   cout << func(4) << endl;
   cout << func(3.4) << endl;
   return 0;
}