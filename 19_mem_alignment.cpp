#include <iostream>
#include <new>       // 必须包含此头文件，才有 std::align_val_t 和 placement new
#include <cstdint>   // 为了使用 uintptr_t 打印地址

// 假设这是一个需要进行 AVX-512 向量计算的矩阵块
// 强行要求 64 字节对齐（一个标准 CPU 缓存行 L1 Cache Line 的大小）
struct alignas(64) AVXMatrixBlock {
    float data[16]; // 16 * 4 = 64 字节

    AVXMatrixBlock() {
        std::cout << "  [+] AVXMatrixBlock 构造函数被调用\n";
    }
    ~AVXMatrixBlock() {
        std::cout << "  [-] AVXMatrixBlock 析构函数被调用\n";
    }
};

int main() {
    // =====================================================================
    // 方式一：现代 C++ 的魔法（日常开发推荐）
    // 只要你用了 alignas，编译器会自动在底层帮你填好 align_val_t 并调用正确的 API
    // =====================================================================
    std::cout << "=== 1. 编译器自动处理超对齐的 new / delete ===\n";
    
    AVXMatrixBlock* auto_ptr = new AVXMatrixBlock();
    
    // 验证一下地址是不是 64 的倍数（16进制地址末位通常是 00, 40, 80, C0）
    uintptr_t addr1 = reinterpret_cast<uintptr_t>(auto_ptr);
    std::cout << "  -> 自动分配的内存地址: 0x" << std::hex << addr1 << std::dec 
              << " (对齐检查: " << (addr1 % 64 == 0 ? "完美对齐 64 字节" : "对齐失败!") << ")\n";
              
    delete auto_ptr; // 编译器在这里默默调用了带 align_val_t 的 delete


    std::cout << "\n=== 2. 极其硬核的底层手写方式 (你要的例子) ===\n";
    // =====================================================================
    // 方式二：手写内存分配器/游戏引擎底层的真实写法
    // 拆解内存分配、对象构造、对象析构、内存释放的全部 4 个步骤
    // =====================================================================

    // 步骤 A：仅仅分配内存（不调用构造函数）
    // 显式传入大小，以及我们要求的对齐参数 std::align_val_t(64)
    void* raw_memory = ::operator new(
        sizeof(AVXMatrixBlock), 
        std::align_val_t(alignof(AVXMatrixBlock))
    );
    
    uintptr_t addr2 = reinterpret_cast<uintptr_t>(raw_memory);
    std::cout << "  -> 手动分配的生内存地址: 0x" << std::hex << addr2 << std::dec 
              << " (对齐检查: " << (addr2 % 64 == 0 ? "完美对齐 64 字节" : "对齐失败!") << ")\n";

    // 步骤 B：原地构造对象（Placement new）
    // 告诉编译器：“别去堆上找新内存了，就在 raw_memory 这块地址上，给我调用构造函数！”
    AVXMatrixBlock* manual_ptr = new(raw_memory) AVXMatrixBlock();

    // ... 在这里使用 manual_ptr 进行极速的矩阵运算 ...

    // 步骤 C：手动析构对象
    // 因为是你强行 placement new 出来的，所以绝对不能用 delete，必须显式调用析构函数
    manual_ptr->~AVXMatrixBlock();

    // 步骤 D：手动释放对齐的内存（高潮来了）
    // 必须，且只能调用带有 std::align_val_t 的释放 API！
    // 这里的参数完美对称：地址、大小、对齐要求
    ::operator delete(
        raw_memory, 
        sizeof(AVXMatrixBlock), 
        std::align_val_t(alignof(AVXMatrixBlock))
    );

    std::cout << "  -> 手动释放内存完毕\n";

    return 0;
}