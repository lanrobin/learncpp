#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include <unordered_map>
#include "absl/container/flat_hash_map.h"

using namespace std;
using namespace std::chrono;

// 压测模板函数：可以接收任何类型的 Map
template <typename MapType>
void run_benchmark(const string& map_name, const vector<uint64_t>& keys_to_insert, const vector<uint64_t>& keys_to_query) {
    MapType m;
    
    // --- 1. 测试插入性能 ---
    auto start_insert = high_resolution_clock::now();
    for (auto key : keys_to_insert) {
        m[key] = key; 
    }
    auto end_insert = high_resolution_clock::now();

    // --- 2. 测试查询性能 ---
    uint64_t dummy_sum = 0; // 故意做一个累加，防止聪明的编译器把查询循环直接优化删掉
    
    auto start_lookup = high_resolution_clock::now();
    for (auto key : keys_to_query) {
        auto it = m.find(key);
        if (it != m.end()) {
            dummy_sum += it->second;
        }
    }
    auto end_lookup = high_resolution_clock::now();

    // --- 输出结果 ---
    cout << "========== " << map_name << " ==========\n";
    cout << "插入耗时 (Insert) : " 
         << duration_cast<milliseconds>(end_insert - start_insert).count() << " ms\n";
    cout << "查询耗时 (Lookup) : " 
         << duration_cast<milliseconds>(end_lookup - start_lookup).count() << " ms\n";
    cout << "(防止优化的无用校验值: " << dummy_sum << ")\n\n";
}

int main() {
    const size_t NUM_INSERT = 2'500'000; // 插入 250 万条数据
    const size_t NUM_QUERY  = 5'000'000; // 查询 500 万条数据

    cout << "正在生成随机测试数据，请稍候...\n";
    vector<uint64_t> keys_to_insert(NUM_INSERT);
    vector<uint64_t> keys_to_query(NUM_QUERY);

    // 使用高性能随机数引擎
    mt19937_64 rng(12345); 
    
    for (size_t i = 0; i < NUM_INSERT; ++i) {
        keys_to_insert[i] = rng();
    }
    for (size_t i = 0; i < NUM_QUERY; ++i) {
        keys_to_query[i] = rng(); // 查询的数据有一半不存在于表中，测试 Miss 性能
    }

    cout << "数据准备完毕，开始压测！\n\n";

    // 压测 1: 传统拉链法 C++ 标准库
    run_benchmark<std::unordered_map<uint64_t, uint64_t>>(
        "std::unordered_map", keys_to_insert, keys_to_query);

    // 压测 2: 现代开放寻址法 Google Abseil
    run_benchmark<absl::flat_hash_map<uint64_t, uint64_t>>(
        "absl::flat_hash_map", keys_to_insert, keys_to_query);

    return 0;
}