#include <iostream>
#include <string>
#include <type_traits> // 必须包含这个头文件才能使用 SFINAE 工具
#include <concepts>    // C++20 的 Concepts 库

// =========================================================================
// 理论课：什么是 SFINAE？
// SFINAE 是 "Substitution Failure Is Not An Error" 的缩写。
// 翻译过来就是：“（模板）替换失败，并不是一个编译错误”。
//
// 编译器在看到你调用一个模板函数时，会去尝试“套用（替换）”所有的模板重载。
// 如果在套用某个模板时，发现语法不对（比如要求必须是整数，你却给了个小数），
// 编译器【不会马上报错停机】，而是默默地把这个不合格的模板从备选名单里划掉，
// 然后继续去尝试下一个模板。
//
// 以前的 C++ 大佬们就是利用这个“偷偷划掉”的特性，来控制该调用哪个函数的。
// =========================================================================


// =========================================================================
// 【第一幕：C++11/14/17 时代的 SFINAE (老方法)】
// 工具：std::enable_if_t 
// 作用：它就像一个保安。如果条件为真，它就放行（变成 void 类型）；
//       如果条件为假，它就“自爆”（没有类型），故意制造一次“替换失败”。
// =========================================================================

// 老版本重载 1：只接待【整数】
template <typename T>
std::enable_if_t<std::is_integral_v<T>> print_number_old(T val) {
    std::cout << "[SFINAE 老魔法] 这是一个整数，值为: " << val << "\n";
}

// 老版本重载 2：只接待【浮点数 (小数)】
template <typename T>
std::enable_if_t<std::is_floating_point_v<T>> print_number_old(T val) {
    std::cout << "[SFINAE 老魔法] 这是一个浮点数，值为: " << val << "\n";
}


// =========================================================================
// 【第二幕：C++20 时代的 Concepts (新方法)】
// 工具：requires 关键字 和 Concept 库
// 作用：直接用大白话说出你的规矩，丢掉上面那一长串难懂的 enable_if。
// =========================================================================

// 新版本重载 1：只接待【整数】(使用 requires 约束)
template <typename T>
requires std::is_integral_v<T>
void print_number_new(T val) {
    std::cout << "[C++20 新语法] 这是一个整数，值为: " << val << "\n";
}

// 新版本重载 2：只接待【浮点数】(使用究极简写语法：auto前直接加 Concept)
// 注：std::floating_point 是 <concepts> 库自带的概念
void print_number_new(std::floating_point auto val) {
    std::cout << "[C++20 新语法] 这是一个浮点数，值为: " << val << "\n";
}


// =========================================================================
// 【测试主程序】
// =========================================================================
int main() {
    std::cout << "--- 测试 1：传入整数 42 ---\n";
    // 故事是这样发生的：
    // 编译器看到 print_number_old(42)。42 是 int。
    // 去看老版本重载 1：is_integral_v<int> 是 true，保安放行，匹配成功！
    // 去看老版本重载 2：is_floating_point_v<int> 是 false，保安自爆，替换失败（SFINAE 生效），划掉名单。
    // 最终调用重载 1。
    print_number_old(42);
    print_number_new(42);
    
    std::cout << "\n--- 测试 2：传入浮点数 3.14 ---\n";
    // 故事是这样发生的：
    // 编译器看到 print_number_old(3.14)。3.14 是 double。
    // 去看老版本重载 1：is_integral_v<double> 是 false，保安自爆，替换失败（SFINAE 生效），划掉名单！
    // 去看老版本重载 2：is_floating_point_v<double> 是 true，保安放行，匹配成功！
    // 最终调用重载 2。
    print_number_old(3.14);
    print_number_new(3.14);

    std::cout << "\n--- 测试 3：传入乱七八糟的类型 (比如字符串) ---\n";
    std::string s = "Hello";
    
    // 取消下面任何一行的注释，都会导致编译报错，因为没有任何一个模板能匹配字符串。
    //print_number_old(s); 
    // print_number_new(s); 
    
    /*
     【极其重要的报错对比体验】：
     如果你取消注释 print_number_old(s) 并编译，编译器会吐出一大堆像天书一样的报错：
     "no type named 'type' in 'struct std::enable_if<false, void>'..."
     （因为 SFINAE 本质上是在玩弄底层类型系统，报错根本不说人话）

     如果你取消注释 print_number_new(s) 并编译，C++20 编译器会非常温柔且明确地告诉你：
     "constraints not satisfied: std::is_integral_v<std::string> is false"
     "constraints not satisfied: std::string does not satisfy std::floating_point"
     （因为 C++20 原生支持了约束，报错直指核心！）
    */

    return 0;
}