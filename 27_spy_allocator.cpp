#include <iostream>
#include <list>
#include <memory_resource>

// 1. 继承 pmr::memory_resource，制作一个“窃听器”
class SpyResource : public std::pmr::memory_resource {
protected:
    void* do_allocate(std::size_t bytes, std::size_t alignment) override {
        // 核心魔法：拦截申请，打印出 list 究竟要多大的内存！
        std::cout << "[窃听] std::list 申请了一个节点，真实大小: " << bytes << " 字节\n";
        
        // 打印完后，老老实实找系统要内存
        return std::pmr::new_delete_resource()->allocate(bytes, alignment);
    }

    void do_deallocate(void* p, std::size_t bytes, std::size_t alignment) override {
        std::pmr::new_delete_resource()->deallocate(p, bytes, alignment);
    }

    bool do_is_equal(const std::pmr::memory_resource& other) const noexcept override {
        return this == &other;
    }
};

int main() {
    SpyResource spy;
    
    // 把我们的窃听器安插进 list 里
    std::pmr::list<int> my_list(&spy);
    
    std::cout << "准备插入第一个元素...\n";
    my_list.push_back(42); 
    
    return 0;
}