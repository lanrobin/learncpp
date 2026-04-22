#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <ranges>

struct User {
    std::string name;
    int age;
};

void print_nums(const std::string& prefix, const std::vector<int>& nums) {
    std::cout << prefix;
    for (int n : nums) std::cout << n << " ";
    std::cout << "\n";
}

int main() {
    std::cout << "========== C++20 之前 vs Ranges 终极对比 ==========\n\n";

    // ====================================================
    // 场景 1：消除冗余的 begin() 和 end()
    // ====================================================
    std::cout << "--- 场景 1：排序与翻转 ---\n";
    std::vector<int> data1 = {7, 3, 9, 1, 5, 2};
    std::vector<int> data2 = {7, 3, 9, 1, 5, 2};

    // ❌ C++20 之前 (Old Way)
    // 痛点：必须手写 begin 和 end，容易写错（比如写成 data1.begin(), data2.end() 导致崩溃）
    std::sort(data1.begin(), data1.end());
    std::reverse(data1.begin(), data1.end());
    print_nums("Old Way 结果: ", data1);

    // ✅ C++20 Ranges (New Way)
    // 爽点：直接把容器本身当成一个 Range 传进去，安全且极简。
    std::ranges::sort(data2);
    std::ranges::reverse(data2);
    print_nums("New Way 结果: ", data2);
    std::cout << "\n";


    // ====================================================
    // 场景 2：基于特定属性操作 (投影 Projections)
    // ====================================================
    std::cout << "--- 场景 2：按年龄给 User 排序 ---\n";
    std::vector<User> users1 = {{"Alice", 30}, {"Bob", 20}, {"Charlie", 25}};
    std::vector<User> users2 = users1;

    // ❌ C++20 之前 (Old Way)
    // 痛点：哪怕只是想比较一个简单的 int 成员，也必须手写一个又臭又长的 Lambda 表达式。
    std::sort(users1.begin(), users1.end(), [](const User& a, const User& b) {
        return a.age < b.age;
    });
    std::cout << "Old Way 第一个人的年龄: " << users1[0].age << "\n";

    // ✅ C++20 Ranges (New Way)
    // 爽点：使用投影（第 3 个参数）。直接告诉编译器“提取 age 这个成员来比”，无需 Lambda。
    std::ranges::sort(users2, std::less<>{}, &User::age);
    std::cout << "New Way 第一个人的年龄: " << users2[0].age << "\n\n";


    // ====================================================
    // 场景 3：流水线处理 (过滤 -> 转换 -> 截断)
    // ====================================================
    std::cout << "--- 场景 3：找出偶数 -> 平方 -> 取前 3 个 ---\n";
    std::vector<int> stream_data = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    // ❌ C++20 之前 (Old Way)
    // 痛点：如果我们想用声明式的 <algorithm> 算法，就会产生大量的【中间临时容器】，
    // 极大地浪费内存和 CPU（非惰性，1到10全部被计算了）。
    std::cout << "[Old Way 执行过程]\n";
    
    std::vector<int> evens;
    // 1. 遍历所有元素，把偶数拷出来（产生新内存申请）
    std::copy_if(stream_data.begin(), stream_data.end(), std::back_inserter(evens), 
                 [](int n) { return n % 2 == 0; });
                 
    std::vector<int> squares;
    // 2. 遍历所有偶数，计算平方（再次产生新内存申请）
    std::transform(evens.begin(), evens.end(), std::back_inserter(squares), 
                   [](int n) { return n * n; });
                   
    std::cout << "Old Way 结果: ";
    // 3. 手动截取前 3 个
    size_t count = std::min<size_t>(3, squares.size());
    for (size_t i = 0; i < count; ++i) {
        std::cout << squares[i] << " ";
    }
    std::cout << "\n\n";


    // ✅ C++20 Ranges (New Way)
    // 爽点 1：声明式管道 `|` 组装，代码意图非常连贯，像读英语句子。
    // 爽点 2：惰性求值 (Lazy Evaluation)，0 内存分配！
    std::cout << "[New Way 执行过程]\n";
    namespace rv = std::views;

    auto result = stream_data 
                | rv::filter([](int n) { 
                      std::cout << "  (检查 " << n << ") "; 
                      return n % 2 == 0; 
                  })
                | rv::transform([](int n) { return n * n; })
                | rv::take(3); // 短路器：拿够 3 个就强制停机！

    std::cout << "New Way 结果: ";
    // 直到这里触发 for 循环，上面的流水线才真正开始运作！
    for (int n : result) {
        std::cout << n << " ";
    }
    std::cout << "\n(请观察上面的日志：因为只要 3 个结果，查到 6 就停止了，7~10 被完美忽略！)\n";

    return 0;
}