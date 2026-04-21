#include <iostream>
#include <memory>
#include <thread>
#include <chrono>

class Worker : public std::enable_shared_from_this<Worker> {
public:
    Worker(int id) : id_(id) {}

    void start_async_monitor() {
        // 使用 weak_from_this() 获取弱引用
        // 这样后台线程就不会增加引用计数，不会阻止 Worker 被销毁
        std::weak_ptr<Worker> weak_self = weak_from_this();

        std::thread([weak_self]() {
            for (int i = 0; i < 5; ++i) {
                std::this_thread::sleep_for(std::chrono::milliseconds(500));

                // 尝试提升为 shared_ptr
                if (auto self = weak_self.lock()) {
                    std::cout << "Worker " << self->id_ << " 还在运行，进度: " << i << std::endl;
                } else {
                    std::cout << "Worker 已经被销毁了，监控线程退出。" << std::endl;
                    break;
                }
            }
        }).detach();
    }

    ~Worker() { std::cout << "Worker " << id_ << " 彻底销毁了。" << std::endl; }

private:
    int id_;
};

int main() {
    {
        auto p = std::make_shared<Worker>(101);
        p->start_async_monitor();

        // 让它跑一会儿
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << "作用域即将结束，p 将被重置。" << std::endl;
    } // p 离开作用域，Worker 被销毁

    // 等待后台线程打印最后的状态
    std::this_thread::sleep_for(std::chrono::seconds(2));
    return 0;
}