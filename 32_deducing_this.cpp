
#include <string>
#include <iostream>
#include <utility>

class StringWrapperBefore23 {
    std::string data_;
public:
    StringWrapperBefore23(std::string str) : data_(std::move(str)) {}
public:
    // 需要为每种情况写一个重载
    std::string&       get() &       { 
        std::cout << "get() & called\n";
        
        return data_; 
    }
    const std::string& get() const & {
        std::cout << "get() const & called\n";
        return data_; 
    }
    std::string&&      get() &&      { 
        std::cout << "get() && called\n";
        return std::move(data_); 
    }
    const std::string&& get() const && { 
        std::cout << "get() const && called\n";
        return std::move(data_); 
    }
};

class StringWrapperCXX23 {
    std::string data_;
public:
    StringWrapperCXX23(std::string str) : data_(std::move(str)) {}
public:
    // C++23 允许我们直接在成员函数中使用 this 的值类别来区分重载
    template<typename Self>
    auto && get(this Self&& self) {
        std::cout << "get() with deduced this called\n";
        return std::forward<Self>(self).data_;
    }
};

int main() {
    StringWrapperBefore23 sw{"Hello"};

    // 需要根据对象的值类别来选择正确的重载
    std::cout << sw.get() << "\n";           // 调用 get() & 版本
    std::cout << std::as_const(sw).get() << "\n"; // 调用 get() const & 版本
    std::cout << std::move(sw).get() << "\n";     // 调用 get() && 版本
    std::cout << std::move(std::as_const(sw)).get() << "\n"; // 调用 get() const && 版本

    StringWrapperCXX23 sw23{"World"};
    std::cout << sw23.get() << "\n";           // 调用 get() & 版本
    std::cout << std::as_const(sw23).get() << "\n"; // 调用 get() const & 版本
    std::cout << std::move(sw23).get() << "\n";     // 调用 get() && 版本
    std::cout << std::move(std::as_const(sw23)).get() << "\n"; // 调用 get() const && 版本
    return 0;
}