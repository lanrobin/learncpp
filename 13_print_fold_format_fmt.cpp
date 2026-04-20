#include <iostream>
#include <utility>
#include <array>
// 如果是 C++20 以后，推荐使用 <format> 或 fmt 库
#include <format> // 或 #include <fmt/ranges.h>

#define USE_FMT_LIBRARY

#ifdef USE_FMT_LIBRARY
#include <fmt/ranges.h>
#endif

template<typename T, T... ints>
void print_with_modern_format(std::integer_sequence<T, ints...>) {
    
    // 1. 利用大括号初始化，把参数包在编译期零开销转成 std::array
    constexpr std::array<T, sizeof...(ints)> arr = { ints... };
    
    // 2. 如果你的项目引入了 fmt 库，一行代码直接秒杀分隔符痛点！
    // 此时根本不需要操心什么最后一个加号的问题，底层帮你处理得干干净净
#ifdef USE_FMT_LIBRARY
    std::cout << fmt::format("{}", fmt::join(arr, " + ")) << '\n';
#elif __cpp_lib_format >= 202207L
    // 3. C++23 的 std::format 也支持 ranges join，但目前的实现还不够完善，可能需要等到 C++23 的编译器和标准库完全支持后才能使用
    std::cout << std::format("{}", std::ranges::join(arr, " + ")) << '\n';
#else
    // 4. 如果没有 fmt 库，也没有完全支持 C++23 的 std::format，那么只能手动处理了，虽然麻烦，但也不是不能实现
    for (size_t i = 0; i < arr.size(); ++i) {
        std::cout << arr[i];
        if (i != arr.size() - 1) {
            std::cout << " + ";
        }
    }
    std::cout << '\n';
#endif
}

int main() {
    print_with_modern_format(std::integer_sequence<int, 1, 2, 3>{});
    print_with_modern_format(std::integer_sequence<int, 4, 5, 6, 7>{});
    return 0;
}