#include <iostream>
#include <vector>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/epoll.h> // epoll 核心头文件

using namespace std;

const int MAX_EVENTS = 1024; // epoll_wait 每次最多返回的事件数
const int PORT = 8888;

// 工具函数：将文件描述符设置为非阻塞模式
// 在 epoll 编程中，无论是监听 Socket 还是客户端 Socket，通常都需要设置为非阻塞
int set_non_blocking(int fd) {
    int old_option = fcntl(fd, F_GETFL);
    int new_option = old_option | O_NONBLOCK;
    fcntl(fd, F_SETFL, new_option);
    return old_option;
}

// 工具函数：将文件描述符添加到 epoll 实例中进行监控
void add_fd_to_epoll(int epoll_fd, int fd) {
    epoll_event event;
    event.data.fd = fd; 
    // EPOLLIN: 监控可读事件
    // EPOLLET: 使用边缘触发模式 (Edge Triggered)，这是 epoll 高效的秘诀之一
    event.events = EPOLLIN | EPOLLET; 
    
    // epoll_ctl：用于注册、修改或删除要监控的文件描述符
    // EPOLL_CTL_ADD：表示添加监控
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &event);
    
    // 添加到 epoll 后，务必将其设置为非阻塞
    set_non_blocking(fd);
}

int main() {
    // 1. 创建服务端的监听 Socket
    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd == -1) {
        perror("socket creation failed");
        return -1;
    }

    // 允许端口复用，防止重启服务器时出现 "Address already in use"
    int opt = 1;
    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(PORT);

    if (bind(listen_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("bind failed");
        return -1;
    }

    if (listen(listen_fd, SOMAXCONN) == -1) {
        perror("listen failed");
        return -1;
    }

    cout << "=== epoll 服务器已启动，监听端口: " << PORT << " ===" << endl;

    // ---------------------------------------------------------
    // 2. epoll 核心初始化
    // ---------------------------------------------------------
    
    // epoll_create1(0) 创建一个 epoll 实例（内核中的一个数据结构，通常是一棵红黑树 + 一个双向链表）
    // 返回值是一个专门用来操作这个 epoll 实例的文件描述符
    int epoll_fd = epoll_create1(0);
    if (epoll_fd == -1) {
        perror("epoll_create1 failed");
        return -1;
    }

    // 将服务端的监听 Socket 添加到 epoll 中，让内核帮我们盯着“有没有新客户端连接”
    add_fd_to_epoll(epoll_fd, listen_fd);

    // 创建一个数组，用来接收内核传回来的“就绪事件”
    vector<epoll_event> events(MAX_EVENTS);

    // ---------------------------------------------------------
    // 3. 事件循环 (Event Loop) - 服务器的心脏
    // ---------------------------------------------------------
    while (true) {
        // epoll_wait: 进程在这里阻塞，直到有 Socket 发生了事件，或者超时。
        // 第三个参数是用来装事件的数组，第四个是最大数量，第五个是超时时间（-1表示永久阻塞直到有事件）
        // 返回值 num_events 表示到底有几个 Socket 发生了事件。
        int num_events = epoll_wait(epoll_fd, events.data(), MAX_EVENTS, -1);

        if (num_events == -1) {
            perror("epoll_wait failed");
            break;
        }

        // 遍历所有发生了事件的 Socket
        for (int i = 0; i < num_events; ++i) {
            int current_fd = events[i].data.fd;

            // 情况 A: 监听 Socket 发生了事件 -> 说明有新的客户端发起了连接请求
            if (current_fd == listen_fd) {
                sockaddr_in client_addr;
                socklen_t client_addr_len = sizeof(client_addr);
                
                // 接受连接
                int client_fd = accept(listen_fd, (struct sockaddr*)&client_addr, &client_addr_len);
                if (client_fd == -1) {
                    perror("accept failed");
                    continue;
                }

                // 将新来的客户端 Socket 也扔进 epoll 里，让内核盯着它“有没有发数据过来”
                add_fd_to_epoll(epoll_fd, client_fd);
                
                char client_ip[INET_ADDRSTRLEN];
                inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
                cout << "[新连接] 客户端 " << client_ip << ":" << ntohs(client_addr.sin_port) 
                     << " 已连接 (fd: " << client_fd << ")" << endl;
            } 
            // 情况 B: 普通客户端 Socket 发生了可读事件 (EPOLLIN) -> 说明客户端发数据过来了
            else if (events[i].events & EPOLLIN) {
                char buf[1024];
                memset(buf, 0, sizeof(buf));
                
                // 由于我们使用了 ET (边缘触发) 模式和非阻塞 Socket，必须用循环把数据读干净
                while (true) {
                    ssize_t bytes_read = recv(current_fd, buf, sizeof(buf) - 1, 0);
                    
                    if (bytes_read > 0) {
                        cout << "[收到数据] fd " << current_fd << ": " << buf;
                        // Echo 回复：将收到的数据原封不动发回去
                        send(current_fd, buf, bytes_read, 0);
                        memset(buf, 0, sizeof(buf)); // 清空 buffer 继续读
                    } 
                    else if (bytes_read == 0) {
                        // 返回 0 说明客户端优雅地关闭了连接
                        cout << "[断开连接] 客户端 fd " << current_fd << " 已断开。" << endl;
                        // 从 epoll 中移除监控 (其实 close 会自动清理，但显式调用是好习惯)
                        epoll_ctl(epoll_fd, EPOLL_CTL_DEL, current_fd, NULL);
                        close(current_fd);
                        break;
                    } 
                    else if (bytes_read == -1) {
                        if (errno == EAGAIN || errno == EWOULDBLOCK) {
                            // 读干净了，没有数据了，跳出循环，等待 epoll_wait 下一次通知
                            break;
                        } else {
                            // 发生其他错误，关闭连接
                            perror("recv error");
                            epoll_ctl(epoll_fd, EPOLL_CTL_DEL, current_fd, NULL);
                            close(current_fd);
                            break;
                        }
                    }
                }
            }
            // 实际项目中还会有 EPOLLOUT (可写) 等事件的处理，此处为了简洁省略
        }
    }

    close(listen_fd);
    close(epoll_fd);
    return 0;
}