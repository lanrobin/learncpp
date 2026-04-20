#include <iostream>


template< typename A, typename B, typename C >
void func(const C & c ) {
    std::cout << "void func(C c )" << std::endl;
}

template< typename A, typename B, typename C >
void funb(B b) {
    std::cout << "void funb(B b )" << std::endl;
}

int main() {
    // 如果模板不能通过参数列表推导出模板参数，那么一定要在尖括号里直接指定模板参数。而且尖括号里指定的模板参数会优先于函数参数列表的推导。
    func<int, double>('x'); // C 被推导为 char
    func<int, double, std::string_view>("x"); // C 被推导为 std::string_view
    funb<int, double, char>(3.14); // B 被推导为 double
    return 0;
}