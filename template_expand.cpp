#include <iostream>
#include <sstream>

// 变长参数模板函数，用于日志记录
template<typename... Args>
void logMessage(const Args&... args) {
    std::ostringstream stream;
    int i = 0;
    int dummy[] = {0, ((stream << args << " "), ++i)...};
    stream << std::endl;
    std::cout << stream.str();
/*
    for(int j = 0; j < sizeof(dummy)/sizeof(int); j++)
    {
        std::cout << "dummy["<<j<<"] = "<<dummy[j]<<std::endl;
    }
    */
}

template<class... Args>
void logMessage17(const Args&... args) {
    std::ostringstream stream;
    int i = 0;
    ((stream << args << " "), ...);
    stream << std::endl;
    std::cout << stream.str();

}

int main() {
    // 使用变长参数模板函数记录日志
    logMessage17("Error:", 404, "- Not Found");
    logMessage("Warning:", "Low battery");
    logMessage("Info:", "User logged in");
    logMessage(1,2,3,4,5,6,7,8);
    return 0;
}
