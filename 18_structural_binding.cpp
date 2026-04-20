#include <iostream>

/*
 * 例子：多值返回
 */
struct S
{
    double num1;
    long num2;
};
 
S foo(int arg1, double arg2)
{
    double result1 = arg1 * arg2;
    long result2 = arg2 / arg1;
    return {result1, result2};//返回结构体S对象
};
 
int main()
{
    auto [num1, num2] = foo(10, 20.2);//自动推导num1为double，num2为long
    std::cout << "num1: " << num1 << ", num2: " << num2 << std::endl;

    struct S snew = foo(20, 34.5);
    std::cout << "snew.num1: " << snew.num1 << ", snew.num2: " << snew.num2 << std::endl;
    
    return 0;
}