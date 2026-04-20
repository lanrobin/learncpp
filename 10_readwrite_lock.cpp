#include <iostream>
#include <shared_mutex>
#include <thread>
#include <mutex>
#include <vector>

struct ThreadSafe {
    mutable std::shared_timed_mutex mutex_;
    const std::chrono::seconds timeout_{1};
    int value_;

    ThreadSafe() {
        value_ = 0;
    }

    int get() const {
        std::shared_lock<std::shared_timed_mutex> lock(mutex_, timeout_);
        return value_;
    }

    void increase() {
        std::unique_lock<std::shared_timed_mutex> lock(mutex_);
        value_ += 1;
    }
};

int main() {
    ThreadSafe ts;
    std::thread t1([&ts] {
        for (int i = 0; i < 10; ++i) {
            ts.increase();
            std::cout << "Thread:" << std::this_thread::get_id() << " Increased value: " << ts.get() << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    });

    std::vector<std::thread> readers;
    for (int i = 0; i < 5; ++i) {
        readers.emplace_back([&ts] {
            for (int j = 0; j < 10; ++j) {
                std::cout << "Thread:" << std::this_thread::get_id() << " Read value: " << ts.get() << std::endl;
                std::this_thread::sleep_for(std::chrono::milliseconds(25));
            }
        });
    }
    for (auto& reader : readers) {
        reader.join();
    }

    t1.join();

    return 0;
}