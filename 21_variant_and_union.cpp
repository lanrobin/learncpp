#include <iostream>
#include <string>
#include <variant>

// -----------------------------------------
// 噩梦模式：使用传统的 union 存放 string
// -----------------------------------------
union OldUnion {
    int i;
    std::string s;

    // 你必须手写构造和析构，极其繁琐且极易出错
    OldUnion() : i(0) {}
    ~OldUnion() {} // 这里不能随便析构 s，因为你不知道当前活跃的是不是 s！
};

void test_union() {
    OldUnion u;
    // 强行构造 string：必须使用 Placement new
    new (&u.s) std::string("Hello");
    std::cout << u.s << "\n";
    // 切换回 int 前，必须手动调用 string 的析构函数！

    std::cout << "这行是未定义行为！因为 s 还活着，你不能直接访问 i" <<u.i << "\n"; // 这行是未定义行为！因为 s 还活着，你不能直接访问 i
    u.s.~basic_string(); 
    u.i = 42;

    std::cout << u.i << "\n";
}

// -----------------------------------------
// 极客模式：使用 C++17 std::variant
// -----------------------------------------
void test_variant() {
    // 干净、优雅、全自动
    std::variant<int, std::string> v = "Hello"; 
    std::cout << std::get<std::string>(v) << "\n";

    std::cout << "访问int会怎么样？" << std::get_if<int>(&v) << "\n"; // 访问int会怎么样？std::bad_variant_access 异常  
    
    // 直接赋值即可！variant 在底层会自动帮你析构之前的 string，极其安全！
    v = 42; 
}

// 现代 C++ 常用的黑魔法：重载器 (Overloader)
template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };

/**
（推导向导 CTAD）
 这叫做 类模板参数推导向导 (Class Template Argument Deduction Guide, 简称 CTAD)。
 你并没有在 overloaded 后面加尖括号 < > 告诉编译器这俩 lambda 是什么类型。事实上你也写不出来，因为 lambda 的类型名是编译器瞎编的（比如 __Lambda_1）。
这行代码就是告诉编译器一个推导规则：
“当你看到我用几个变量来初始化 overloaded 时，这几个变量是什么类型，你就自动把 Ts... 推导成什么类型！”

(注：在 C++20 中，编译器变得更聪明了，这第二行代码可以省略不写，但在 C++17 中必须写)。
 */
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

int main() {

    std::cout << "sizeof(OldUnion): " << sizeof(OldUnion) << "\n"; // sizeof(OldUnion): 32 (通常是 string 的大小)

    test_union();
    test_variant();

    std::variant<int, double, std::string> var_double = 3.14;
    std::variant<int, double, std::string> var_string = "Hello, Variant!";

    auto visitor = overloaded {
        [](int i) { std::cout << "It's an int: " << i << "\n"; },
        [](double d) { std::cout << "It's a double: " << d << "\n"; },
        [](const std::string& s) { std::cout << "It's a string: " << s << "\n"; }
    };

    // 无论 var 当前是什么类型，visit 都会自动路由到对应的 lambda
    std::visit(visitor, var_double); // 输出: It's a double: 3.14
    std::visit(visitor, var_string); // 输出: It's a string: Hello, Variant!

    return 0;
}