#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <new>


/**
 
sudo apt update
sudo apt install libtcmalloc-minimal4 libgoogle-perftools-dev libjemalloc-dev -y
sudo apt install 

 */

// 定义一个典型的业务对象大小 (比如 64 字节，刚好一个 Cache Line)
struct alignas(64) Payload {
    uint64_t data[8];
};

const int NUM_THREADS = 12;         // 模拟 12 核心并发
const int ALLOC_PER_THREAD = 500000; // 每个线程分配 50 万次

// ==========================================
// 测试 A：传统无大小释放 ::operator delete(void*)
// ==========================================
void bench_unsized_delete() {
    std::vector<Payload*> ptrs(ALLOC_PER_THREAD);
    
    // 1. 疯狂分配
    for (int i = 0; i < ALLOC_PER_THREAD; ++i) {
        ptrs[i] = new Payload; 
    }
    
    // 2. 传统释放 (只传指针)
    for (int i = 0; i < ALLOC_PER_THREAD; ++i) {
        ::operator delete(ptrs[i]); 
    }
}

// ==========================================
// 测试 B：现代大小感知释放 ::operator delete(void*, size_t)
// ==========================================
void bench_sized_delete() {
    std::vector<Payload*> ptrs(ALLOC_PER_THREAD);
    
    // 1. 疯狂分配
    for (int i = 0; i < ALLOC_PER_THREAD; ++i) {
        ptrs[i] = new Payload; 
    }
    
    // 2. 大小感知释放 (传入精确大小)
    for (int i = 0; i < ALLOC_PER_THREAD; ++i) {
        ::operator delete(ptrs[i], sizeof(Payload)); 
    }
}

// 执行多线程压测的包装函数
template <typename Func>
void run_benchmark(const std::string& name, Func func) {

    std::cout << "=== 热身 " << name << " ===\n";
    // warm-up: 先单线程跑一次，避免冷启动影响结果
    func();
    
    std::cout << "=== 开始测试: " << name << " ===\n";

    auto start = std::chrono::high_resolution_clock::now();

    std::vector<std::thread> threads;
    for (int i = 0; i < NUM_THREADS; ++i) {
        threads.emplace_back(func);
    }
    for (auto& t : threads) {
        t.join();
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end - start;
    std::cout << "[ " << name << " ] 耗时: " << elapsed.count() << " ms\n";
}

int main() {
    std::cout << "=== 内存分配器高并发压测 ===\n";
    std::cout << "线程数: " << NUM_THREADS << ", 每线程分配次数: " << ALLOC_PER_THREAD << "\n\n";

    run_benchmark("现代大小感知释放 (Sized)  ", bench_sized_delete);
    run_benchmark("传统无大小释放 (Unsized)", bench_unsized_delete);
    return 0;
}