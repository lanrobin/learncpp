#include <utility>
#include <string>
#include <iostream>

class MyBuffer{
    std::string data_;
public:
    // 移动构造函数，使用成员初始化列表，极其优雅！
    MyBuffer(MyBuffer&& other) noexcept
        : data_(std::exchange(other.data_, "")) {
            std::cout << "MyBuffer(MyBuffer&& other) called, data_ swapped with empty string\n";
        } 
        // 一行代码干了两件事：
        // 1. 用 "" 替换了 other.data_ (成功掏空对方)
        // 2. 把 other.data_ 替换前的旧指针返回，初始化给了自己的 data_

    // 没有exchange函数的版本，手动实现交换逻辑
    // MyBuffer(MyBuffer&& other) {
    //     data_ = other.data_; // 先把对方的指针拿过来
    //     other.data_ = nullptr; // 再把对方的指针掏空，防止 double free
    // }

    MyBuffer(const std::string& str) : data_(str) {}

    MyBuffer(const MyBuffer&) = default;
    MyBuffer& operator=(const MyBuffer&) = default;

    MyBuffer& operator=(MyBuffer&& other) noexcept {
        if (this != &other) {
            data_ = std::exchange(other.data_, "");
        }
        return *this;
    }

    void print() const {
        std::cout << "Buffer data: " << (data_.empty() ? "empty" : data_) << std::endl;
    }
};

int main() {
    MyBuffer buf1("buf1!");
    MyBuffer buf2("buf2!"); 



    buf1.print(); // 输出 buf1!
    buf2.print(); // 输出 buf2!

    std::swap(buf1, buf2); // 交换 buf1 和 buf2 的内容
    std::cout << "After swap:\n";
    buf1.print(); // 输出 buf2!
    buf2.print(); // 输出 buf1!

    MyBuffer bufnew = std::exchange(buf1, MyBuffer("new buffer!")); // buf1 交换成一个新对象，原来的 buf1 内容被返回给 bufnew
    std::cout << "After exchange:\n";
    buf1.print(); // 输出 new buffer!
    bufnew.print(); // 输出 buf2! (原来 buf1 的内容)

    return 0;
}