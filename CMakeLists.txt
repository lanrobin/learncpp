cmake_minimum_required(VERSION 3.10)
project(LearnCpp)

# 设定 C++ 标准为 C++17（或者 20）
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 魔法开始：自动查找当前目录下所有的 .cpp 文件
file(GLOB SOURCES "*.cpp")

# 循环遍历每个 .cpp 文件
foreach(source IN LISTS SOURCES)
    # 提取不带后缀的文件名 (例如把 "01_hello.cpp" 变成 "01_hello")
    get_filename_component(target_name ${source} NAME_WE)
    
    # 为每个文件单独生成一个可执行文件
    add_executable(${target_name} ${source})
endforeach()