#include <iostream>
#include <string_view>
#include <ranges>
#include <vector>
#include <algorithm>

int main() {
    std::string_view text = "apple,banana,cherry";
    
    // C++20 优雅的视图分割，完全不需要正则！零内存分配！
    for (auto word : text | std::views::split(',')) {
        std::cout << std::string_view(word.begin(), word.end()) << "\n";
    }

    //===========传统c++写法===========
    std::vector<int> nums = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    // 1. 过滤偶数 (被迫产生一个临时的大 vector)
    std::vector<int> evens;
    std::copy_if(nums.begin(), nums.end(), std::back_inserter(evens), 
                 [](int x) { return x % 2 == 0; });

    // 2. 算平方 (又被迫产生一个临时 vector)
    std::vector<int> squares;
    std::transform(evens.begin(), evens.end(), std::back_inserter(squares), 
                   [](int x) { return x * x; });

    // 3. 取前 3 个
    std::vector<int> result(squares.begin(), squares.begin() + std::min(3UL, squares.size()));

    for (int x : result) std::cout << x << " "; // 输出: 4 16 36
    std::cout << "\n";

    //===========C++20 ranges写法===========
    // 像拼积木一样组合逻辑
    auto ranges_result = nums 
                | std::views::filter([](int x) { return x % 2 == 0; })
                | std::views::transform([](int x) { return x * x; })
                | std::views::take(3);

    for (int x : ranges_result) {
        std::cout << x << " "; // 输出: 4 16 36
    }
    std::cout << "\n";

    // ===View 的另一个用法

    const std::vector<std::tuple<int, char, std::string_view, std::string_view>> vt
    {
        {1, 'A', "α", "甲"},
        {2, 'B', "β", "乙"},
        {3, 'C', "γ", "丙"},
        {4, 'D', "δ", "丁"},
        {5, 'E', "ε", "戊"},
    };
    
    for (int const e : std::views::elements<0>(vt))
        std::cout << e << ' ';
    std::cout << '\n';
    
    for (char const e : vt | std::views::elements<1>)
        std::cout << e << ' ';
    std::cout << '\n';
    
    for (std::string_view const& e : std::views::elements<2>(vt))
        std::cout << e << ' ';
    std::cout << '\n';
    
    for (const auto& e : std::views::elements<3>(vt))
        std::cout << e << ' ';
    std::cout << '\n';
    return 0;
}