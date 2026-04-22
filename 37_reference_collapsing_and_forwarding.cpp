#include <iostream>
#include <string>
#include <utility>
#include <type_traits>

// ====================================================
// 探针类：用于监控到底发生了拷贝还是移动
// ====================================================
class Tracker {
public:
    std::string name;

    Tracker(const std::string& n) : name(n) {
        std::cout << "  [构造] " << name << " 诞生了\n";
    }

    // 拷贝构造
    Tracker(const Tracker& other) : name(other.name + "_copy") {
        std::cout << "  [拷贝构造] (耗时操作) 复制了 " << other.name << " 变成 " << name << "\n";
    }

    // 移动构造
    Tracker(Tracker&& other) noexcept : name(std::move(other.name) + "_moved") {
        std::cout << "  [移动构造] (零开销) 偷取了老对象，变成 " << name << "\n";
    }
};

// ====================================================
// 场景 1 & 2：万能引用与完美转发 (Perfect Forwarding)
// 问题：在写泛型底层容器时，如何保证数据绝对不发生多余的拷贝？
// ====================================================

// 假设这是我们要初始化的底层容器节点
struct Node {
    Tracker payload;
    // 节点构造函数接收万能引用
    template <typename T>
    Node(T&& val) : payload(std::forward<T>(val)) {} 
    // 【核心点】：这里如果不用 std::forward，而是直接用 val，
    // 因为 val 本身是有名字的变量（是个左值），它会无脑触发拷贝构造！
    // 即使你外层传进来的是一个右值。
};

// 这是一个泛型的工厂函数（类似 std::make_unique 或 vector::emplace_back 的底层）
// 【核心点】：使用 T&& 接收参数，这叫万能引用 (Forwarding Reference)
template <typename T>
void makeNode_Bad(T&& arg) {
    std::cout << "  -- [反面教材] 不使用完美转发 --\n";
    // 错误用法 1：直接传 arg。因为 arg 有名字，是左值，必定触发拷贝。
    // 错误用法 2：无脑 std::move(arg)。如果外部传的是不可偷的左值，你把它强行偷走了，外部代码会出严重 Bug！
    Node node(arg); 
}

template <typename T>
void makeNode_Perfect(T&& arg) {
    std::cout << "  -- [正确示范] 使用 std::forward 完美转发 --\n";
    // std::forward 配合万能引用 T&&，能完美保留 arg 最初进来的“左值”或“右值”属性！
    Node node(std::forward<T>(arg)); 
}


// ====================================================
// 测试主函数
// ====================================================
int main() {
    std::cout << "========== 实验 1：传入左值 (lvalue) ==========\n";
    Tracker t1("Lvalue_Obj");
    
    // t1 是左值，T 推导为 Tracker&。
    // 根据引用折叠：Tracker& && -> Tracker&。
    // 万能引用变成了左值引用。
    makeNode_Bad(t1);      // 触发拷贝
    makeNode_Perfect(t1);  // std::forward 发现 T 是左值引用，不作为，保留左值属性，触发【拷贝】（符合预期，因为不能随便偷左值）


    std::cout << "\n========== 实验 2：传入右值 (prvalue/xvalue) ==========\n";
    // Tracker("Rvalue_Obj") 是临时对象，是个纯右值。
    // T 推导为 Tracker。
    // 万能引用变成了右值引用 (Tracker&&)。
    makeNode_Bad(Tracker("Rvalue_Obj1")); // 【惨剧发生】：本来是右值，但在 makeNode_Bad 内部，
                                          // 参数有了名字 arg（变成了左值），结果底层依然触发了【拷贝构造】！浪费了性能！

    makeNode_Perfect(Tracker("Rvalue_Obj2")); // 【完美】：std::forward 发现 T 不是左值引用，
                                              // 于是将其强转回右值，底层成功匹配【移动构造】！零多余拷贝！


    std::cout << "\n========== 实验 3：揭秘底层实现机理 ==========\n";
    std::cout << "std::move 和 std::forward 到底是什么？\n";
    std::cout << "在汇编代码层面，它们什么都不是。在编译期，它们的源码如下：\n\n";
    
    /* 【std::move 的简化版源码】：无脑去引用 + 强转 &&
    template <typename T>
    constexpr std::remove_reference_t<T>&& move(T&& arg) noexcept {
        return static_cast<std::remove_reference_t<T>&&>(arg);
    }
    
    【std::forward 的简化版源码】：条件判断强转
    template <typename T>
    constexpr T&& forward(std::remove_reference_t<T>& arg) noexcept {
        // 返回类型 T&& 会触发引用折叠！
        // 如果模板参数 T 是左值引用 (Tracker&)，返回 Tracker& &&，折叠为 Tracker&
        // 如果模板参数 T 是非引用类型 (Tracker) ，返回 Tracker&&
        return static_cast<T&&>(arg);
    }
    */
    std::cout << "因此，在运行期，这俩函数连 1 bit 的额外内存和 1 个 CPU 时钟周期的开销都不会产生。\n";

    return 0;
}