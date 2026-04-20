#include <iostream>
#include <string_view>
#include <vector>
#include "absl/strings/str_split.h"

#define USE_ABSL_SPLIT

#if __cplusplus >= 202002L
    // C++20 已经正式支持 ranges 了，直接包含头文件就行
    #include <ranges>
#endif

std::vector<std::string_view> split_string(const std::string_view &str, const std::string_view &delimiter) 

#if defined(USE_ABSL_SPLIT)
{
    std::cout << "使用 Abseil 的 StrSplit 进行字符串分割\n";
    std::vector<std::string_view> result = absl::StrSplit(str, delimiter);
    return result;
    
}
#elif __cplusplus >= 202002L
{
    std::cout << "使用 C++20 ranges 进行字符串分割\n";
    std::vector<std::string_view> result;
    for (auto part : str | std::views::split(delimiter)) {
        result.emplace_back(part.begin(), part.end());
    }
    return result;
}
#else
{
    std::cout << "使用传统 C++ 方法进行字符串分割\n";
    std::vector<std::string_view> result;
    size_t start = 0;
    size_t end = str.find(delimiter);

    while (end != std::string::npos) {
        result.push_back(str.substr(start, end - start));
        start = end + delimiter.length();
        end = str.find(delimiter, start);
    }
    // 把最后一段塞进去
    result.push_back(str.substr(start));
    return result;
}
#endif

int main() {
    //std::string_view text = "apple,banana,cherry";
    //std::string_view delimiter = ",";
    std::string_view text = "one___two___three___four";
    std::string_view delimiter = "___";
    
    auto parts = split_string(text, delimiter);
    
    for (const auto& part : parts) {
        std::cout << part << "\n";
    }
    
    return 0;
}