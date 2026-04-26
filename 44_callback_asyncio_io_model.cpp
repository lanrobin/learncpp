#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <aio.h>
#include <thread>
#include <chrono>

using namespace std;

// 模拟客户端：5秒后连接并发送数据
void start_delayed_client(int port) {

    std::cout << "[客户端] 延迟 5 秒后准备连接服务器..., thread ID: " << std::this_thread::get_id() << std::endl;
    this_thread::sleep_for(chrono::seconds(5));
    
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);

    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == 0) {
        string msg = "hello async IO with callback!";
        send(sock, msg.c_str(), msg.length(), 0);
        cout << "\n[客户端] 数据已发送!" << endl;
    }
    close(sock);
}

// ---------------------------------------------------------
// 回调函数：当异步 I/O 完成时，系统会在一个新的后台线程中调用它
// ---------------------------------------------------------
void aio_completion_handler(__sigval_t sigval) {
    // 1. 从传递过来的指针恢复 aiocb 结构体
    struct aiocb *req = (struct aiocb *)sigval.sival_ptr;

    std::cout << "\n>>> [回调线程] 回调函数被调用，当前线程 ID: " << std::this_thread::get_id() << std::endl;

    // 2. 检查异步操作是否真的成功完成了
    if (aio_error(req) == 0) {
        // 3. 获取实际读取到的字节数
        ssize_t bytes_read = aio_return(req);
        if (bytes_read > 0) {
            cout << "\n>>> [回调线程] 收到内核通知！异步读取完成。" << endl;
            cout << ">>> [回调线程] 读取到的内容: " << (char*)req->aio_buf << endl;
            cout << ">>> [回调线程] 回调函数执行完毕，准备退出系统。" << endl;
        }
    } else {
        cerr << "\n>>> [回调线程] 异步读取发生错误!" << endl;
    }
    
    // 注意：在实际工程中，通常需要在这里清理分配的内存 (如 req->aio_buf 和 req 本身)
}

// 初始化服务端 Socket (与前面相同)
int setup_server(int port) {
    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);
    bind(listen_fd, (struct sockaddr*)&addr, sizeof(addr));
    listen(listen_fd, 5);
    return listen_fd;
}

int main() {
    int port = 9999;
    int listen_fd = setup_server(port);
    cout << "[主线程] 服务器启动，监听端口: " << port << endl;

    // 启动延迟 5 秒发送数据的客户端
    jthread client_thread(start_delayed_client, port);
    //client_thread.detach();

    // 接收连接 (为简单起见，这里 accept 是阻塞的，真正的纯异步服务器 accept 也要处理)
    cout << "[主线程] 等待客户端连接..., thread ID: " << std::this_thread::get_id() << std::endl;
    sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    int conn_fd = accept(listen_fd, (struct sockaddr*)&client_addr, &client_len);
    cout << "[主线程] 客户端已连接，准备发起带回调的异步读取..." << endl;

    // ---------------------------------------------------------
    // 配置并发起真正的异步 I/O 请求
    // ---------------------------------------------------------
    char buf[1024];
    memset(buf, 0, sizeof(buf));

    struct aiocb cb;
    memset(&cb, 0, sizeof(cb));
    cb.aio_fildes = conn_fd;
    cb.aio_buf = buf;
    cb.aio_nbytes = sizeof(buf) - 1;

    // === 核心配置：设置异步通知方式为“回调函数” ===
    cb.aio_sigevent.sigev_notify = SIGEV_THREAD;                  // 通知方式：新起一个线程执行回调
    cb.aio_sigevent.sigev_notify_function = aio_completion_handler; // 指定回调函数
    cb.aio_sigevent.sigev_notify_attributes = NULL;               // 线程属性（使用默认）
    cb.aio_sigevent.sigev_value.sival_ptr = &cb;                  // 将控制块指针传给回调函数，以便在回调中获取数据

    // 发起异步读请求 (瞬间返回)
    if (aio_read(&cb) == -1) {
        cerr << "发起异步读取失败" << endl;
        return 1;
    }

    cout << "[主线程] 异步请求已提交给内核！主线程现在完全自由，可以去做其他事情了。\n" << endl;

    // 主线程干别的事情，**完全不去检查 aio_error**
    for (int i = 1; i <= 8; ++i) {
        cout << "[主线程] 正在处理其他业务逻辑... (" << i << " 秒)" << endl;
        this_thread::sleep_for(chrono::seconds(1));
    }

    close(conn_fd);
    close(listen_fd);
    cout << "\n[主线程] 演示结束。" << endl;
    return 0;
}