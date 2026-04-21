#include <array>
#include <vector>
#include <chrono>
#include <cstddef>
#include <iomanip>
#include <iostream>
#include <list>
#include <memory_resource>

constexpr int iterations{100};
constexpr int total_nodes{1'000'000};

// 全局 Buffer：32MB 足够装下 100万个 list 节点或 vector 元素
std::vector<std::byte> global_buffer(total_nodes * 32); 

template<typename Func>
auto benchmark(Func test_func, int iterations)
{
    const auto start = std::chrono::system_clock::now();
    while (iterations-- > 0)
        test_func();
    const auto stop = std::chrono::system_clock::now();
    const auto secs = std::chrono::duration<double>(stop - start);
    return secs.count();
}

// ==========================================
// 1. 标准分配 (默认 std::allocator)
// ==========================================
template <typename Container>
void test_std_alloc() {
    Container c;
    // C++20 魔法：如果这个容器类型支持 reserve()，就在编译期生成这段代码！
    if constexpr (requires { c.reserve(1); }) {
        c.reserve(total_nodes);
    }
    for (int i{}; i != total_nodes; ++i)
        c.push_back(i);
}

// ==========================================
// 2. PMR 默认分配 (new_delete_resource)
// ==========================================
template <typename PmrContainer>
void test_pmr_default() {
    PmrContainer c;
    if constexpr (requires { c.reserve(1); }) {
        c.reserve(total_nodes);
    }
    for (int i{}; i != total_nodes; ++i)
        c.push_back(i);
}

// ==========================================
// 3. PMR 无全局 Buffer (内部自动扩张的单调缓冲)
// ==========================================
template <typename PmrContainer>
void test_pmr_no_buf() {
    std::pmr::monotonic_buffer_resource mbr;
    std::pmr::polymorphic_allocator<int> pa{&mbr};
    PmrContainer c{pa};
    if constexpr (requires { c.reserve(1); }) {
        c.reserve(total_nodes);
    }
    for (int i{}; i != total_nodes; ++i)
        c.push_back(i);
}

// ==========================================
// 4. PMR + 全局 Buffer (终极性能形态)
// ==========================================
template <typename PmrContainer>
void test_pmr_and_buf() {
    // 每次循环进来，都白嫖外面的 global_buffer
    std::pmr::monotonic_buffer_resource mbr{global_buffer.data(), global_buffer.size()};
    std::pmr::polymorphic_allocator<int> pa{&mbr};
    PmrContainer c{pa};
    if constexpr (requires { c.reserve(1); }) {
        c.reserve(total_nodes);
    }
    for (int i{}; i != total_nodes; ++i)
        c.push_back(i);
}

// ==========================================
// 统一的测试套件运行器
// ==========================================
template <typename StdContainer, typename PmrContainer>
void run_test_suite(const std::string& name) {
    std::cout << "========== 测试容器: " << name << " ==========\n";
    // 注意：把模板函数当作参数传给 benchmark
    const double t1 = benchmark(test_std_alloc<StdContainer>, iterations);
    const double t2 = benchmark(test_pmr_default<PmrContainer>, iterations);
    const double t3 = benchmark(test_pmr_no_buf<PmrContainer>, iterations);
    const double t4 = benchmark(test_pmr_and_buf<PmrContainer>, iterations);

    std::cout << std::fixed << std::setprecision(3)
              << "t1 (default std alloc): " << t1 << " sec; t1/t1: " << t1/t1 << '\n'
              << "t2 (default pmr alloc): " << t2 << " sec; t1/t2: " << t1/t2 << '\n'
              << "t3 (pmr alloc  no buf): " << t3 << " sec; t1/t3: " << t1/t3 << '\n'
              << "t4 (pmr alloc and buf): " << t4 << " sec; t1/t4: " << t1/t4 << "\n\n";
}

int main()
{
    // 测试 1: std::list 家族
    run_test_suite<std::list<int>, std::pmr::list<int>>("std::list");

    // 测试 2: std::vector 家族
    run_test_suite<std::vector<int>, std::pmr::vector<int>>("std::vector");

    return 0;
}