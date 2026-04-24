/*
编译器的“选妃”三步曲：
寻找“候选函数” (Candidate Functions)：
编译器首先会在当前作用域内，找出所有名字相同的函数。

筛选“可行函数” (Viable Functions)：
从候选者中，挑出那些参数数量匹配，并且传入的实参可以通过某种类型转换变成形参类型的函数。如果连可行的都没有，直接报编译错误。

决出“最佳匹配” (Best Match)：（最核心的一步）
编译器会根据类型转换的代价给这些可行函数打分。谁的转换代价最小，谁就是赢家。排名的优先级如下（从高到低）：

级别 1：精确匹配 (Exact Match)：参数类型完全一致，或者仅仅是加了 const、数组退化成指针等微小调整。

级别 2：类型提升 (Promotion)：比如 short、char 自动提升为 int，float 提升为 double。

级别 3：标准类型转换 (Standard Conversion)：比如 int 转 double，double 转 int，指针转换等（可能会丢失精度）。

级别 4：类类型转换 (Class-type Conversion)：通过类的构造函数或类型转换运算符进行的转换。
*/

#include <iostream>
#include <string>

// ====================================================
// 第一组重载：演示基本匹配和类型提升
// ====================================================
void process(int x) {
    std::cout << "[匹配 1] process(int) 被调用\n";
}

void process(double x) {
    std::cout << "[匹配 2] process(double) 被调用\n";
}

// ====================================================
// 第二组重载：演示二义性陷阱 (多参数匹配)
// ====================================================
void calculate(int x, double y) {
    std::cout << "[匹配 3] calculate(int, double) 被调用\n";
}

void calculate(double x, int y) {
    std::cout << "[匹配 4] calculate(double, int) 被调用\n";
}

// ====================================================
// 第三组重载：演示 const 引用与精确匹配
// ====================================================
// 1号选手：非常量左值引用 (接纳普通变量)
void print(std::string& s) {
    std::cout << "[匹配 1] std::string&      -> 赢家：非常量左值\n";
}

// 2号选手：常量左值引用 (万能备胎，啥都能接，但优先级低)
void print(const std::string& s) {
    std::cout << "[匹配 2] const std::string& -> 赢家：常量左值\n";
}

// 3号选手：非常量右值引用 (本次的主角！)
void print(std::string&& s) {
    std::cout << "[匹配 3] std::string&&      -> 赢家：普通右值 (绝对王者！)\n";
}

// 4号选手：常量右值引用 (语法合法，但工程中几乎不用)
void print(const std::string&& s) {
    std::cout << "[匹配 4] const std::string&&-> 赢家：常量右值 (极其罕见)\n";
}

// 辅助函数：制造一个常量右值
const std::string makeConstRValue() {
    return "Const Temporary";
}

int main() {
    std::cout << "========== 场景 A：基本类型的较量 ==========\n";
    
    // 1. 精确匹配
    // 传入的字面量 42 是 int 类型。
    // 候选：process(int) [级别1: 精确匹配] 和 process(double) [级别3: 标准转换]
    // 赢家：process(int)
    process(42); 

    // 2. 精确匹配
    // 传入的字面量 3.14 默认是 double 类型。
    // 候选：process(int) [级别3: 标准转换] 和 process(double) [级别1: 精确匹配]
    // 赢家：process(double)
    process(3.14); 

    // 3. 类型提升 (Promotion) vs 标准转换 (Standard Conversion)
    short s = 10;
    // short 转换为 int 属于“类型提升”(级别 2)。
    // short 转换为 double 属于“标准转换”(级别 3)。
    // 级别 2 优于 级别 3。
    // 赢家：process(int)
    process(s); 

    // 4. float 的类型提升
    float f = 2.5f;
    // float 转换为 double 属于“类型提升”(级别 2)。
    // float 转换为 int 属于“标准转换”(级别 3，会丢失小数部分)。
    // 赢家：process(double)
    process(f); 


    std::cout << "\n========== 场景 B：令人头疼的二义性 ==========\n";
    
    // 5. 毫无悬念的匹配
    calculate(10, 3.14); // 精确匹配 calculate(int, double)

    // 6. 二义性错误 (Ambiguous Call)
    // calculate(10, 20); 
    // 【编译器内心崩溃】：
    // 传入了 (int, int)。
    // 如果选 calculate(int, double)：第一个参数完美匹配，第二个参数需要 标准转换 (int -> double)。
    // 如果选 calculate(double, int)：第一个参数需要 标准转换 (int -> double)，第二个参数完美匹配。
    // 两个候选者各自在一个参数上占优，谁也无法彻底打败谁！
    // 【结果】：如果你取消这行的注释，编译器会直接报错：call of overloaded 'calculate(int, int)' is ambiguous。
    // 【解决办法】：必须显式强转，比如 calculate(10, static_cast<double>(20));


    std::cout << "\n========== 场景 C：const 引用的优先级 ==========\n";
    
   std::string str1 = "Hello";
    const std::string str2 = "World";

    std::cout << "--- 测试 1：传入非常量左值 (str1) ---\n";
    // 3号和4号出局（左值不能绑定到 &&）。
    // 1号和2号竞争，1号（无需添加 const）胜出。
    print(str1); // 输出：[匹配 1]

    std::cout << "\n--- 测试 2：传入常量左值 (str2) ---\n";
    // 1号、3号出局（不能丢失 const）。
    // 4号出局（左值不能绑定到 &&）。
    // 只有2号独霸天下。
    print(str2); // 输出：[匹配 2]

    std::cout << "\n--- 测试 3：传入普通右值 (临时对象) ---\n";
    // 字符串字面量 "Temp" 会隐式构造一个【普通的、非 const】的 std::string 临时对象。
    // 1号出局（右值不能绑定到普通左值引用）。
    // 2号、3号、4号全部可行！
    // 【终极对决】：
    // vs 2号：3号是 &&，右值优先绑定 &&，3号胜。
    // vs 4号：临时对象是非 const 的，绑定到 4号需要额外加上 const 修饰符。而绑定到 3号是 100% 精确匹配。
    // 最终 3号 碾压胜出！
    print("Temp"); // 输出：[匹配 3]

    std::cout << "\n--- 测试 4：传入常量右值 ---\n";
    // 我们用辅助函数强行制造了一个带 const 的临时对象。
    // 1号、3号出局（不能丢失 const 保护）。
    // 2号和4号可行。
    // 4号是 &&，专门接右值，优先级高于 2号。
    print(makeConstRValue()); // 输出：[匹配 4] 
    
    // 或者对一个 const 左值进行 std::move，也会产生 const 右值
    print(std::move(str2)); // 同样输出：[匹配 4]

    return 0;
}

/*
在实际的 C++ 标准库和企业级代码中，我们绝大多数情况只会留下 2号 和 3号。

保留 const T&（2号）： 用来作为默认的只读传参方式，或者作为拷贝构造函数（Copy Constructor）的签名。

保留 T&&（3号）： 用来作为移动语义的专属通道，也就是移动构造函数（Move Constructor）和移动赋值运算符的签名。

如果你写了一个类，里面只有这两个版本（const T& 和 T&&）：

当你传入左值时，它会走 const T&（触发深拷贝）。

当你传入右值（临时变量，或 std::move 后的变量）时，它会精准地走 T&&（触发零开销的偷窃/移动）。

这就是现代 C++ 高性能的基石！
*/