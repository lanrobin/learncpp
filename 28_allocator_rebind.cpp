#include <iostream>
#include <list>

// 一个简单的间谍分配器
template <typename T>
struct SpyAllocator {
    using value_type = T;

    SpyAllocator() = default;

    // 允许被 rebind 强行转换成其他类型的分配器
    template <typename U>
    SpyAllocator(const SpyAllocator<U>&) noexcept {}

    T* allocate(std::size_t n) {
        // 此时的 T，已经被 list 底层 rebind 成了它真正的内部 Node 类型！
        std::cout << "[传统窃听] 当前需要分配的 T 的大小: " << sizeof(T) << " 字节，数量: " << n << '\n';
        return static_cast<T*>(::operator new(n * sizeof(T)));
    }

    void deallocate(T* p, std::size_t n) noexcept {
        std::cout << "[传统窃听] 当前需要释放的 T 的大小: " << sizeof(T) << " 字节，数量: " << n << '\n';
        ::operator delete(p, n * sizeof(T));
    }
};

int main() {
    // 虽然我们传进去的是 SpyAllocator<int>
    std::list<int, SpyAllocator<int>> my_list;
    
    std::cout << "准备插入元素...\n";
    my_list.push_back(100);

    return 0;
}