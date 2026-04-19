#include <tuple>
#include <string>

// 1. 声明一个模板类，但【不提供】具体实现
template <typename T>
struct DumpType; 

int main() {
    auto t1 = std::make_tuple(1, 3.14);
    auto t2 = std::make_tuple("Hello", 'A');
    auto mega_tuple = std::tuple_cat(t1, t2);

    // 2. 故意使用这个未定义的模板，触发编译错误！
    // DumpType<decltype(mega_tuple)> dumper; 

    return 0;
}