#include<iostream>

int main() {
    int a = 0;

    auto f = [=] () mutable {
        a += 1; // 这里虽然修改了 a，但并不会影响外部的 a，因为捕获的是 a 的副本
        std::cout << "Lambda 内部的 a: " << a << "\n";
    };

    f(); // 输出: Lambda 内部的 a: 1
    std::cout << "Lambda 外部的 a: " << a << "\n"; // 输出: Lambda 外部的 a: 0

    return 0;
}