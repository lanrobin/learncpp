#include <functional>
#include <iostream>

int cook(int a, int b) {
    std::cout << a << " + " << b << " = " << a+b << std::endl;
    return a + b;
}

int add_three(int a, int b, int c) {
    std::cout << a << " + " << b << " + " << c << " = " << a+b+c << std::endl;
    return a + b + c;
}

struct Multiplier {
    int operator()(int a, int b) const {
        std::cout << a << " * " << b << " = " << a * b << std::endl;
        return a * b;
    }
};

int main()
{
    // 1. 普通函数
    std::function<int(int, int)> func = cook; // 函数名自动退化成函数指针
    func(3, 5); // 输出：3 + 5 = 8

    // 2. 函数指针
    int (*funcPtr)(int,int) = cook;   // 定义函数指针
    std::function<int(int,int)> func2 = funcPtr;
    func2(7, 2); // 输出：7 + 2 = 9

    // 3. Lambda 表达式
    auto lambda = [](int a, int b) {
    std::cout << a << " * " << b << " = " << a * b << std::endl;
    return a * b;
    };
    std::function<int(int, int)> func3 = lambda;
    func3(4, 6); // 输出：4 * 6 = 24

    // 4. 函数对象（重载了 operator() 的类实例）
    Multiplier multiplier;
    std::function<int(int, int)> func4 = multiplier;
    func4(2, 3); // 输出：2 * 3 = 6

    // 5. std::bind 绑定函数参数
    auto boundfunc = std::bind(add_three, 1, std::placeholders::_1, 3); // 绑定第一个参数为 1，第三个参数为 3
    std::function<int(int)> func5 = boundfunc;
    func5(2); // 输出：1 + 2 + 3 = 6

    return 0;
}