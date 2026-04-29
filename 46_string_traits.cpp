#include <iostream>
#include <string>
#include <cctype>   // 用于 char 的 toupper
#include <cwctype>  // 用于 wchar_t 的 towupper

// 1. 声明泛化版本的模板（可以不实现，或者给一个默认实现）
template <typename CharT>
struct ci_traits; 

// ==========================================
// 2. 针对 char 的特化版本
// ==========================================
template <>
struct ci_traits<char> : std::char_traits<char> {
    static bool eq(char c1, char c2) { return std::toupper(c1) == std::toupper(c2); }
    static bool lt(char c1, char c2) { return std::toupper(c1) < std::toupper(c2); }
    static int compare(const char* s1, const char* s2, size_t n) {
        while (n-- != 0) {
            if (std::toupper(*s1) < std::toupper(*s2)) return -1;
            if (std::toupper(*s1) > std::toupper(*s2)) return 1;
            ++s1; ++s2;
        }
        return 0;
    }
    static const char* find(const char* s, int n, char a) {
        auto target = std::toupper(a);
        while (n-- != 0) {
            if (std::toupper(*s) == target) return s;
            ++s;
        }
        return nullptr;
    }
};

// ==========================================
// 3. 针对 wchar_t 的特化版本
// ==========================================
template <>
struct ci_traits<wchar_t> : std::char_traits<wchar_t> {
    // 注意这里换成了 towupper
    static bool eq(wchar_t c1, wchar_t c2) { return std::towupper(c1) == std::towupper(c2); }
    static bool lt(wchar_t c1, wchar_t c2) { return std::towupper(c1) < std::towupper(c2); }
    static int compare(const wchar_t* s1, const wchar_t* s2, size_t n) {
        while (n-- != 0) {
            if (std::towupper(*s1) < std::towupper(*s2)) return -1;
            if (std::towupper(*s1) > std::towupper(*s2)) return 1;
            ++s1; ++s2;
        }
        return 0;
    }
    static const wchar_t* find(const wchar_t* s, int n, wchar_t a) {
        auto target = std::towupper(a);
        while (n-- != 0) {
            if (std::towupper(*s) == target) return s;
            ++s;
        }
        return nullptr;
    }
};

// ==========================================
// 4. 定义通用的自定义字符串类型模板
// ==========================================
template <typename CharT>
using basic_ci_string = std::basic_string<CharT, ci_traits<CharT>>;

// 导出常用的别名
using ci_string = basic_ci_string<char>;
using ci_wstring = basic_ci_string<wchar_t>;

int main() {
    ci_string s1 = "Hello";
    ci_string s2 = "hElLo";
    std::cout << "char 版本比较: " << (s1 == s2 ? "相等" : "不相等") << "\n";

    ci_wstring ws1 = L"World"; // L 前缀表示宽字符
    ci_wstring ws2 = L"wOrLd";
    std::cout << "wchar_t 版本比较: " << (ws1 == ws2 ? "相等" : "不相等") << "\n";

    return 0;
}