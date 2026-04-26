#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <aio.h>
#include <cstring>
#include <cerrno>
#include <thread>
#include <chrono>

using namespace std;

// ==============================================================================
// 1. 同步阻塞 I/O (Blocking I/O)
// 特点：调用 read() 时，如果无数据可读，当前线程会被挂起（睡眠），直到有数据到来。
// ==============================================================================
void demonstrate_blocking_io() {
    cout << "\n--- 1. 演示同步阻塞 I/O ---" << endl;
    cout << "系统已阻塞，请在终端输入一些内容并按回车..." << endl;

    char buf[128];
    memset(buf, 0, sizeof(buf));

    // read 系统调用会一直等待，直到终端有输入
    ssize_t bytes_read = read(STDIN_FILENO, buf, sizeof(buf) - 1);

    if (bytes_read > 0) {
        cout << "阻塞读取成功！读取到的内容: " << buf;
    } else {
        cerr << "读取出错" << endl;
    }
}

// ==============================================================================
// 2. 同步非阻塞 I/O (Non-blocking I/O)
// 特点：调用 read() 时，如果无数据，立即返回错误(EAGAIN/EWOULDBLOCK)，线程不会被挂起。
// 应用程序通常需要在一个循环中不断轮询（Polling）。
// ==============================================================================
void demonstrate_nonblocking_io() {
    cout << "\n--- 2. 演示同步非阻塞 I/O ---" << endl;
    
    // 获取当前标准输入的标志位
    int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    // 设置 O_NONBLOCK 标志，将其变为非阻塞模式
    fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);

    cout << "已设置为非阻塞模式。请在 5 秒内输入内容..." << endl;

    char buf[128];
    memset(buf, 0, sizeof(buf));
    int attempts = 0;

    // 轮询 (Polling) 检查是否有数据
    while (true) {
        ssize_t bytes_read = read(STDIN_FILENO, buf, sizeof(buf) - 1);

        if (bytes_read > 0) {
            cout << "\n非阻塞读取成功！读取到的内容: " << buf;
            break;
        } else if (bytes_read == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
            // EAGAIN 意味着 "当前没有数据，请重试"。此时线程没有被阻塞，可以做其他事情。
            cout << "尝试读取 (" << ++attempts << ")：暂无数据，CPU去执行其他任务..." << endl;
            this_thread::sleep_for(chrono::seconds(1)); // 模拟执行其他任务耗时1秒
            
            if (attempts >= 5) {
                cout << "超时未输入，结束非阻塞演示。" << endl;
                break;
            }
        } else {
            cerr << "发生严重错误" << endl;
            break;
        }
    }

    // 恢复标准输入为阻塞模式，以免影响后续演示
    fcntl(STDIN_FILENO, F_SETFL, flags);
}

// ==============================================================================
// 3. 异步 I/O (Asynchronous I/O - 使用 POSIX AIO)
// 特点：向内核提交一个 I/O 请求后立即返回。内核在后台完成读写操作，完成后通知应用程序。
// 应用程序完全不需要轮询读写本身。
// ==============================================================================
void demonstrate_async_io() {
    cout << "\n--- 3. 演示异步 I/O (POSIX AIO) ---" << endl;
    cout << "已发起异步读取请求，请在终端输入内容。主线程将同时处理其他工作..." << endl;

    char buf[128];
    memset(buf, 0, sizeof(buf));

    // 定义并初始化异步 I/O 控制块 (AIO Control Block)
    aiocb cb;
    memset(&cb, 0, sizeof(cb));
    cb.aio_fildes = STDIN_FILENO;   // 监听的文件描述符（标准输入）
    cb.aio_buf = buf;               // 数据存放的缓冲区
    cb.aio_nbytes = sizeof(buf) - 1;// 期望读取的字节数

    // 发起异步读请求 (此函数调用后会立即返回，不会等待用户输入)
    if (aio_read(&cb) == -1) {
        cerr << "异步读取请求发起失败" << endl;
        return;
    }

    // 主线程继续执行，不需要像非阻塞模式那样去调用 read()
    int work_count = 0;
    while (aio_error(&cb) == EINPROGRESS) {
        // EINPROGRESS 表示内核还在后台进行 I/O 操作（等待用户输入）
        cout << "主线程正在处理其他工作... (" << ++work_count << ")" << endl;
        this_thread::sleep_for(chrono::seconds(1));
        
        if (work_count >= 5) {
            cout << "用户太久未输入，取消异步请求。" << endl;
            aio_cancel(STDIN_FILENO, &cb);
            return;
        }
    }

    // 获取异步读取的结果
    ssize_t bytes_read = aio_return(&cb);
    if (bytes_read > 0) {
        cout << "\n内核后台读取完毕！触发通知。读取到的内容: " << (char*)cb.aio_buf;
    } else {
        cerr << "异步读取发生错误" << endl;
    }
}

int main() {
    cout << "=== Linux I/O 模型演示 ===" << endl;
    
    // 1. 演示阻塞模型
    demonstrate_blocking_io();
    
    // 2. 演示非阻塞模型
    demonstrate_nonblocking_io();
    
    // 3. 演示异步模型
    demonstrate_async_io();

    cout << "\n演示结束。" << endl;
    return 0;
}