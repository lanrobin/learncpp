#include <iostream>
#include <string>
#include <vector>
#include <type_traits>

// =========================================================================
// 门派一：返回值 SFINAE (最古老、最直观)
// 陷阱位置：函数的返回值类型
// =========================================================================

// 重载 1：仅限整数
template <typename T>
std::enable_if_t<std::is_integral_v<T>> process_return(T val) {
    std::cout << "[1. 返回值 SFINAE] 这是一个整数: " << val << "\n";
}

// 重载 2：仅限浮点数
template <typename T>
std::enable_if_t<std::is_floating_point_v<T>> process_return(T val) {
    std::cout << "[1. 返回值 SFINAE] 这是一个浮点数: " << val << "\n";
}


// =========================================================================
// 门派二：默认模板参数 SFINAE (工业界最爱，构造函数的救星)
// 陷阱位置：模板的尖括号 < > 内部
// =========================================================================

class IntWrapper {
public:
    // 💡 重点：构造函数是没有返回值的！所以门派一在这里彻底失效。
    // 我们必须把 SFINAE 陷阱埋在模板参数的默认值里。
    // 如果 T 不是整数，`std::enable_if_t` 报错，导致这里的 `typename = 语法错误`，触发淘汰。
    template <typename T, 
              typename = std::enable_if_t<std::is_integral_v<T>>>
    IntWrapper(T val) {
        std::cout << "[2. 模板参数 SFINAE] 构造成功！值是: " << val << "\n";
    }
};


// =========================================================================
// 门派三：函数参数 SFINAE (另辟蹊径)
// 陷阱位置：函数的圆括号 ( ) 内部的形参列表
// =========================================================================

// 悄悄在参数列表最后加一个默认参数（通常是无用的指针）。
// 如果 T 不是整数，这个参数的类型就不存在，函数签名残缺，触发淘汰。
template <typename T>
void process_func_param(T val, std::enable_if_t<std::is_integral_v<T>>* = nullptr) {
    std::cout << "[3. 函数参数 SFINAE] 这是一个整数: " << val << "\n";
}

template <typename T>
void process_func_param(T val, std::enable_if_t<std::is_floating_point_v<T>>* = nullptr) {
    std::cout << "[3. 函数参数 SFINAE] 这是一个浮点数: " << val << "\n";
}


// =========================================================================
// 门派四：表达式 SFINAE (decltype 魔法，C++20 requires 的雏形)
// 陷阱位置：尾随返回类型 (Trailing Return Type)
// 目标：我们不查 type_traits，我们直接测试“这个对象能不能调用 .size()”
// =========================================================================

// 优选版本：尝试推导 obj.size() 的类型。
// 如果 obj 没有 size() 方法，decltype 推导失败，整个函数的返回值非法，触发淘汰！
template <typename T>
auto get_safe_size(const T& obj) -> decltype(obj.size()) {
    std::cout << "[4. 表达式 SFINAE] 完美！对象有 size() 方法，获取成功。\n";
    return obj.size();
}

// 兜底版本：由于 C 语言的变长参数 `...` 在 C++ 重载决议中优先级是最低的，
// 所以只有当上面的优选版本 SFINAE 失败时，编译器才会极其无奈地选择这个版本。
size_t get_safe_size(...) {
    std::cout << "[4. 表达式 SFINAE] 警告：该对象根本没有 size() 方法！返回 0 作为兜底。\n";
    return 0;
}


// =========================================================================
// 主测试程序
// =========================================================================
int main() {
    std::cout << "========== 测试门派一：返回值 ==========\n";
    process_return(42);       // 调用整数版本
    process_return(3.14159);  // 调用浮点版本
    // process_return("Hello"); // 编译报错：找不到匹配的函数（两个都 SFINAE 失败了）

    std::cout << "\n========== 测试门派二：模板参数 (构造函数) ==========\n";
    IntWrapper w1(100);       // 完美编译，100 是 int
    // IntWrapper w2(3.14);   // 编译报错：浮点数触发 SFINAE，且没有兜底的构造函数！
    // IntWrapper w3("Hi");   // 编译报错：字符串触发 SFINAE

    std::cout << "\n========== 测试门派三：函数参数 ==========\n";
    process_func_param(99);   // 调用整数版本
    process_func_param(0.618); // 调用浮点版本

    std::cout << "\n========== 测试门派四：表达式 (鸭子类型) ==========\n";
    std::vector<int> vec = {1, 2, 3};
    std::string str = "Hello C++";
    int normal_number = 42;

    std::cout << "测试 vector: ";
    get_safe_size(vec);  // vector 有 size()，推导成功，进入优选版本

    std::cout << "测试 string: ";
    get_safe_size(str);  // string 有 size()，推导成功，进入优选版本

    std::cout << "测试 普通int: ";
    get_safe_size(normal_number); // int 没有 size()，推导失败！SFINAE 淘汰优选版本，跌落到兜底版本！

    return 0;
}