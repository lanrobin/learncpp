#include <iostream>
#include <type_traits>
#include <concepts>

// 使用 requires 充当门卫，要求 T 必须是整数类型
template <typename T>
requires std::is_integral_v<T> 
T add(T a, T b) {
    return a + b;
}


// 定义一个 Concept (概念)
template <typename T>
concept CanFly = requires(T obj) {
    // 规矩1：必须能调用 take_off()，并且不关心返回值
    obj.take_off(); 
    
    // 规矩2：必须能调用 fly()，并且返回值必须可以隐式转换为 std::string
    { obj.fly() } -> std::convertible_to<std::string>; 
};

// --- 下面是测试类型 ---
struct Bird {
    void take_off() {}
    std::string fly() { return "Bird is flying"; }
};

struct Dog {
    void bark() {}
};

// 写法 1：传统的 requires 子句（最严谨）
template <typename T>
requires CanFly<T>
void make_it_fly1(T entity) {
    std::cout << entity.fly() << "\n";
}

// 写法 2：将 Concept 替代 typename（极其直观）
template <CanFly T>
void make_it_fly2(T entity) {
    std::cout << entity.fly() << "\n";
}

//写法 3：C++20 究极语法 —— 简写函数模板 (Abbreviated Function Template)
// 这就是 C++20 的终极形态：它在底层依然是一个模板函数！
void make_it_fly3(CanFly auto entity) {
    std::cout << entity.fly() << "\n";
}

int main() {
    std::cout << add(10, 20) << "\n";    // 完美运行
    // std::cout << add(3.14, 2.5);      // 编译当场报错：约束未满足！(因为浮点数不是整数)
    // std::cout << add("A", "B");       // 编译当场报错：约束未满足！

    Bird pigeon;
    make_it_fly1(pigeon); // 正确输出: Bird is flying
    make_it_fly2(pigeon); // 正确输出: Bird is flying
    make_it_fly3(pigeon); // 正确输出: Bird is flying

    Dog husky;
    // make_it_fly(husky); // 报错：Dog 没有满足 CanFly 概念 (缺少 take_off 和 fly 方法)

    return 0;
}