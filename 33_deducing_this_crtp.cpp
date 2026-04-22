#include <iostream>

/**
CRTP（Curiously Recurring Template Pattern，奇异递归模板模式） 是 C++ 中一种非常经典且强大的设计模式。

它之所以叫“奇异”或“古怪”，是因为它的代码结构乍一看有点违背直觉：一个子类在继承基类时，把“自己”作为模板参数传给了基类。

CRTP 最大的作用是实现 “静态多态”（编译期多态）。

在传统的面向对象编程中，如果我们想实现多态，通常会使用 virtual 虚函数：

传统虚函数的缺点（动态多态）： 依赖于运行时的虚函数表（vtable）。每次调用虚函数时，程序都要去查表才能决定到底调用哪个子类的方法。这会带来一定的运行时开销，而且阻碍了编译器的内联优化（inlining）。

CRTP 的优点（静态多态）： 借助模板，在编译期就确定了到底要调用哪个子类的方法。不仅没有虚函数的运行时开销，编译器还能做极致的内联优化，性能极高。
*/

template <typename Derived>
struct Base {
public:

    void interface() {
        // 必须强转 this 指针才能调用子类的方法
        static_cast<Derived*>(this)->implementation();
        std::cout << "Base interface\n";
    }
};

struct Derived : Base<Derived> {

public:
    void implementation() {
        std::cout << "Derived implementation\n";
    }
};


struct Base23 {
public:
    void interface(this auto&& self) {
        // C++23 允许我们直接在成员函数中使用 this 的值类别来区分重载
        self.implementation();
        std::cout << "Base23 interface\n";
    }
};

struct Derived23 : Base23 {
public:
    void implementation() {
        std::cout << "Derived23 implementation\n";
    }
};

int main() {
    Derived d;
    d.interface();
    Derived23 d23;
    d23.interface();
    return 0;
}