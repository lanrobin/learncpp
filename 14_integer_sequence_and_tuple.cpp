#include <iostream>
#include <tuple>
#include <utility>
#include <string>

// ==========================================
// 1. 底层实现 (参数包 Is... 作为模板参数)
// ==========================================
template <std::size_t... Is, typename F, typename T>
auto map_filter_tuple(F f, T& t) {
    std::cout << "[命中 函数 1] 'auto map_filter_tuple(F f, T& t)' -> 直接通过数字参数包展开\n";
    return std::make_tuple(f(std::get<Is>(t))...);
}

// ==========================================
// 2. 桥梁层 (通过 index_sequence 实例推导)
// ==========================================
template <std::size_t... Is, typename F, typename T>
auto map_filter_tuple(std::index_sequence<Is...>, F f, T& t) {
    std::cout << "[命中 函数 2] 'auto map_filter_tuple(std::index_sequence<Is...>, F f, T& t)' -> 通过 index_sequence 对象拆解推导\n";
    return std::make_tuple(f(std::get<Is>(t))...);
}

// ==========================================
// 3. API 外观层 (传入序列类型 S)
// ==========================================
template <typename S, typename F, typename T>
auto map_filter_tuple(F&& f, T& t) {
    std::cout << "[命中 函数 3] 'auto map_filter_tuple(F&& f, T& t)' -> 接收类型 S，并向下转发...\n";
    // 注意：这里实例化了 S{}，所以它必定会触发调用函数 2
    return map_filter_tuple(S{}, std::forward<F>(f), t);
}


int main() {
    // 准备测试数据
    std::tuple<int, double, std::string, char> my_tuple(42, 3.14, "Hello", 'A');

    // 准备一个极其简单的映射函数：直接返回原值
    auto f = [](auto&& x) { return x; };

    std::cout << "=== 测试开始 ===\n\n";

    // ---------------------------------------------------------
    // 姿势 A：直接调用【函数 1】
    // ---------------------------------------------------------
    // 魔法解析：在尖括号里直接传数字 <0, 2>。
    // 函数 2 期望 3 个函数参数，排除；
    // 函数 3 期望模板参数是一个类型 (typename S)，但你传了数字 0，替换失败排除；
    // 完美命中函数 1！后面的 F 和 T 由编译器自动从 f 和 my_tuple 推导。
    auto res1 = map_filter_tuple<0, 2>(f, my_tuple);
    std::cout << "结果 1: " << std::get<1>(res1) << "\n\n"; // 输出 Hello


    // ---------------------------------------------------------
    // 姿势 B：直接调用【函数 2】
    // ---------------------------------------------------------
    // 魔法解析：我们在调用时，强行给它塞进去 3 个参数。
    // 第一个参数直接传入一个 std::index_sequence<1, 3> 的【实例对象】（注意后面的 {}）。
    // 函数 1 和函数 3 都只接收 2 个函数参数 (f, t)，直接被淘汰；
    // 完美命中函数 2！
    auto res2 = map_filter_tuple(std::index_sequence<1, 3>{}, f, my_tuple);
    std::cout << "结果 2: " << std::get<1>(res2) << "\n\n"; // 输出 A


    // ---------------------------------------------------------
    // 姿势 C：正常调用【函数 3】(并连锁触发函数 2)
    // ---------------------------------------------------------
    // 魔法解析：这也是最标准的用户用法。在尖括号里传一个【类型】。
    // 函数 1 期望尖括号里是数字，淘汰；
    // 函数 2 期望 3 个函数入参，淘汰；
    // 完美命中函数 3！然后函数 3 内部执行 S{}，产生了一个实例对象，从而触发了函数 2。
    auto res3 = map_filter_tuple<std::index_sequence<0, 3>>(f, my_tuple);
    std::cout << "结果 3: " << std::get<1>(res3) << "\n\n"; // 输出 A

    return 0;
}