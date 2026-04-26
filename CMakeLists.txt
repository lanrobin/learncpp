cmake_minimum_required(VERSION 3.30)

find_program(CLANG_CXX_EXECUTABLE NAMES clang++)

if(CLANG_CXX_EXECUTABLE)
    # 如果找到了 clang++，将其设置为 C++ 编译器
    set(CMAKE_CXX_COMPILER ${CLANG_CXX_EXECUTABLE})
    message(STATUS "Using Clang++: ${CLANG_CXX_EXECUTABLE}")
else()
    # 如果没找到，CMake 默认会寻找 g++ 或系统默认编译器
    message(STATUS "Clang++ not found, using default compiler (usually g++)")
endif()

project(LearnCpp)

# 设定 C++ 标准为 C++23）
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 开启最高级性能优化 (极其重要！)
if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /O2")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
endif()

# 魔法：自动从 GitHub 下载 Abseil 库，不需要你手动安装！
include(FetchContent)
FetchContent_Declare(
  absl
  URL https://github.com/abseil/abseil-cpp/archive/refs/tags/20240116.1.tar.gz
)
FetchContent_MakeAvailable(absl)


# 自动下载 CTRE (新增的)
FetchContent_Declare(
    ctre
    URL https://github.com/hanickadot/compile-time-regular-expressions/archive/refs/tags/v3.8.1.tar.gz
)
FetchContent_MakeAvailable(ctre)


FetchContent_Declare(
    fmt
    URL https://github.com/fmtlib/fmt/archive/refs/tags/10.2.1.tar.gz
)
FetchContent_MakeAvailable(fmt)

find_package(Threads REQUIRED)

# 魔法开始：自动查找当前目录下所有的 .cpp 文件
file(GLOB SOURCES "*.cpp")

# 循环遍历每个 .cpp 文件
foreach(source IN LISTS SOURCES)
    # 提取不带后缀的文件名 (例如把 "01_hello.cpp" 变成 "01_hello")
    get_filename_component(target_name ${source} NAME_WE)
    
    # 为每个文件单独生成一个可执行文件
    add_executable(${target_name} ${source})

    # 关键逻辑：如果你这个文件的名字里包含 "02_unordered_map_perf"
    # 我们就自动给它链接上 Abseil 的哈希表库
    if(${target_name} MATCHES "absl")
        target_link_libraries(${target_name} PRIVATE absl::flat_hash_map)
        message(STATUS "已为目标 [${target_name}] 链接 Abseil 库")
    endif()

    # 规则 B：如果文件名包含 "ctre"，链接 CTRE
    if(${target_name} MATCHES "ctre")
        target_link_libraries(${target_name} PRIVATE ctre)
        message(STATUS "已为目标 [${target_name}] 链接 CTRE 库")
    endif()

    # 规则 C：如果文件名包含 "fmt"，链接 fmt
    if(${target_name} MATCHES "fmt")
        target_link_libraries(${target_name} PRIVATE fmt::fmt)
        message(STATUS "已为目标 [${target_name}] 链接 fmt 库")
    endif()

    if(${target_name} MATCHES "threads")
        target_link_libraries(${target_name} Threads::Threads)
        message(STATUS "已为目标 [${target_name}] 链接线程库")
    endif()

    if(${target_name} MATCHES "io_model")
        target_link_libraries(${target_name} PRIVATE rt Threads::Threads)
        message(STATUS "已为目标 [${target_name}] 链接实时库 librt (POSIX AIO 依赖) 和线程库 Threads::Threads")
    endif()

    if(${target_name} MATCHES "allocator_bench")
        # =================================================================
        # 内存分配器压测配置 (ptmalloc vs tcmalloc vs jemalloc)
        # =================================================================

        # 强制开启 C++14 的 sized-deallocation 优化 (GCC/Clang 默认 C++17 开启，明确写上更安全)
        # add_compile_options(-fsized-deallocation -O3)

        # 寻找系统中的 TCMalloc 和 Jemalloc 库
        find_library(TCMALLOC_LIB tcmalloc)
        find_library(JEMALLOC_LIB jemalloc)

        # 1. 默认分配器目标 (ptmalloc, glibc 自带)
        add_executable(bench_ptmalloc 20_allocator_bench.cpp)
        target_link_libraries(bench_ptmalloc Threads::Threads)
        message(STATUS "已生成目标: bench_ptmalloc (glibc default)")

        # 2. TCMalloc 目标
        if(TCMALLOC_LIB)
            add_executable(bench_tcmalloc 20_allocator_bench.cpp)
            # 静态/动态链接 tcmalloc 库会接管默认的 new/delete
            target_link_libraries(bench_tcmalloc Threads::Threads ${TCMALLOC_LIB})
            message(STATUS "已生成目标: bench_tcmalloc 链接到 ${TCMALLOC_LIB}")
        else()
            message(WARNING "未找到 tcmalloc，跳过 bench_tcmalloc 构建")
        endif()

        # 3. Jemalloc 目标
        if(JEMALLOC_LIB)
            add_executable(bench_jemalloc 20_allocator_bench.cpp)
            target_link_libraries(bench_jemalloc Threads::Threads ${JEMALLOC_LIB})
            message(STATUS "已生成目标: bench_jemalloc 链接到 ${JEMALLOC_LIB}")
        else()
            message(WARNING "未找到 jemalloc，跳过 bench_jemalloc 构建")
        endif()
    endif()
endforeach()
