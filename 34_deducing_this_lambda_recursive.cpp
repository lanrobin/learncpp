#include <functional>
#include <iostream>

// 必须事先声明，并按引用捕获自己
std::function<int(int)> fib = [](int n) {
    if (n <= 1) return n;
    return fib(n - 1) + fib(n - 2);
};

auto fib23 = [](this auto&& self, int n) -> int {
    if (n <= 1) return n;
    return self(n - 1) + self(n - 2);
};

int main() {
    std::cout << fib(10) << std::endl;
    std::cout << fib23(10) << std::endl;
    return 0;
}