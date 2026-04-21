#include <iostream>
#include <any>
#include <string>
#include <vector>

// 一个普通的自定义类
struct MyStruct {
    double data = 9.9;
};

int main() {
    // ==========================================
    // 场景一：单个 std::any 的“单人包厢”行为
    // ==========================================
    std::cout << "--- 场景一：单个 std::any ---\n";
    
    std::any single_box = 42; // 现在里面装的是 int
    std::cout << "装了 int: " << std::any_cast<int>(single_box) << "\n";

    // 随时可以换成别的类型，旧的数据会被自动安全析构！
    single_box = std::string("Hello C++17"); 
    std::cout << "换成 string: " << std::any_cast<std::string>(single_box) << "\n";


    // ==========================================
    // 场景二：std::vector<std::any> 实现“万能杂物箱”
    // ==========================================
    std::cout << "\n--- 场景二：混合装载的 vector ---\n";
    
    std::vector<std::any> mixed_bag;
    
    // 什么都能往里塞！
    mixed_bag.push_back(100);                  // int
    mixed_bag.push_back(3.14159);              // double
    mixed_bag.push_back(std::string("World")); // std::string
    mixed_bag.push_back(MyStruct{});           // 自定义结构体

    // 如何把它们安全地拿出来？
    // 必须通过 .type() 检查类型，然后用 std::any_cast 拆包
    for (const auto& item : mixed_bag) {
        if (item.type() == typeid(int)) {
            std::cout << "找到一个 int: " << std::any_cast<int>(item) << "\n";
        } 
        else if (item.type() == typeid(double)) {
            std::cout << "找到一个 double: " << std::any_cast<double>(item) << "\n";
        } 
        else if (item.type() == typeid(std::string)) {
            std::cout << "找到一个 string: " << std::any_cast<const std::string &>(item) << "\n";
        } 
        else if (item.type() == typeid(MyStruct)) {
            // 注意这里用了const引用 std::any_cast<const MyStruct&>，避免不必要的拷贝
            std::cout << "找到 MyStruct，数据为: " << std::any_cast<const MyStruct&>(item).data << "\n";
        }
    }

    return 0;
}