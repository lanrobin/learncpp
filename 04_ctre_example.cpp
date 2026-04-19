#include <iostream>
#include <string_view>
#include <ctre.hpp>

using namespace std;

// 我们的目标是解析类似 "2026-04-18" 这样的日期，并提取出年、月、日
// 正则表达式: ^([0-9]{4})-([0-9]{2})-([0-9]{2})$
void parse_date(string_view date_str) {
    
    // 【魔法时刻】：整个正则表达式在编译时就被解析成了底层汇编状态机！
    // 语法 ctre::match<"正则">(字符串) 是 C++20 的专属优雅写法
    if (auto match = ctre::match<"^([0-9]{4})-([0-9]{2})-([0-9]{2})$">(date_str)) {
        
        cout << "✅ 成功匹配有效日期: " << date_str << "\n";
        
        // 提取捕获组：.get<N>()
        // 这也是编译期决定的，如果你写了 .get<4>()，而正则里只有 3 个括号，编译会直接报错！
        cout << "   -> 年 (Year)  : " << match.get<1>().to_string() << "\n";
        cout << "   -> 月 (Month) : " << match.get<2>().to_string() << "\n";
        cout << "   -> 日 (Day)   : " << match.get<3>().to_string() << "\n\n";
        
    } else {
        cout << "❌ 日期格式无效: " << date_str << "\n\n";
    }
}

int main() {
    cout << "=== CTRE (编译期正则表达式) 演示 ===\n\n";

    // 1. 测试标准格式
    parse_date("2026-04-18");

    // 2. 测试错误格式 (斜杠分隔)
    parse_date("2026/04/18");

    // 3. 测试错误格式 (包含字母)
    parse_date("2026-04-XX");

    // 4. CTRE 另一个极客用法：编译期验证正则表达式本身是否写错！
    // 假设你手滑，把括号写漏了，下面这行代码如果解除注释，程序将【无法编译】！
    // auto bad_match = ctre::match<"^[0-9]+)">( "123" ); 

    return 0;
}