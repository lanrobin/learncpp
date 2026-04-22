#include <atomic>
#include <cassert>
#include <iostream>
#include <thread>
#include <vector>

// ============================================================================
// 序言：面试官常问的核心破冰问题
// ============================================================================
/*
 * 【面试问题】：为什么 C++ 需要引入 std::memory_order？
 * 【标准回答】：为了在多线程环境下对抗“编译器优化”和“CPU 指令乱序执行”。
 * 在单线程下，指令重排不影响结果；但在多线程下，如果缺乏内存序的约束，
 * 线程 A 写入的数据，线程 B 可能无法按预期顺序看到。
 * memory_order 就是用来告诉编译器和
 * CPU：在这里必须遵守特定的内存可见性规则，不能瞎优化。
 */

// ============================================================================
// 测试 1：第一层级 - 绝对自由 (Relaxed)
// ============================================================================
namespace RelaxedTest {
std::atomic<int> global_counter{0};

/*
 * 【面试问题】：什么是 memory_order_relaxed？它的适用场景是什么？
 * 【标准回答】：Relaxed 模型是开销最小的内存序。它仅仅保证当前这一个操作
 * 本身是“原子”的（不会读到半个字），但它【绝对不保证】线程之间的执行顺序和内存可见性。
 * 编译器和 CPU 可以随意重排它前后的非原子操作。
 * 适用场景：全局计数器（如统计发包总数、shared_ptr 的引用计数增加）。
 * 因为在这些场景里，我们只关心“数字被正确累加”，不关心别的线程在加 1
 * 时还在做什么。
 */
void increment_counter() {
    for (int i = 0; i < 1000; ++i) {
        // 【面试考点】：此处使用 relaxed 是因为这里没有与其他变量的依赖关系。
        global_counter.fetch_add(1, std::memory_order_relaxed);
    }
}

void run() {
    std::cout << "--- 1. Testing memory_order_relaxed ---\n";
    std::vector<std::jthread> threads;
    for (int i = 0; i < 10; ++i) threads.emplace_back(increment_counter);
    std::cout << "Relaxed Counter: " << global_counter.load() << " (Expected: 10000)\n\n";
}
}  // namespace RelaxedTest

// ============================================================================
// 测试 2：第二层级 - 发布与获取 (Release & Acquire)
// ============================================================================
namespace ReleaseAcquireTest {
std::atomic<bool> ready{false};
int payload_data = 0;  // 普通的非原子变量

/*
 * 【面试问题】：Release 和 Acquire 的核心作用是什么？能举个例子吗？
 * 【标准回答】：它们构成了线程间的“同步屏障”，必须成对使用，最经典的就是“生产者-消费者”模型。
 * - Release (发布)：用于写操作。保证排在
 * release【之前】的代码，绝对不能被重排到它【之后】。
 * - Acquire (获取)：用于读操作。保证排在
 * acquire【之后】的代码，绝对不能被重排到它【之前】。
 */
void producer() {
    // 1. 准备普通数据
    payload_data = 42;

    // 2. 用 release 发布标志位
    // 【面试考点】：这就像一道门。编译器/CPU 绝对不允许把 `payload_data = 42`
    // 优化/重排到 `ready.store` 的后面。
    ready.store(true, std::memory_order_release);
}

void consumer() {
    // 1. 用 acquire 等待标志位
    // 【面试问题（进阶）】：如果这里消费者不用 acquire 而用 relaxed 会怎样？
    // 【标准回答】：如果用 relaxed，编译器/CPU 可能会把后面的 `payload_data`
    // 读取操作 提前到 `ready.load` 之前执行。那么消费者可能读到一个未初始化的
    // 0，导致灾难性 BUG！
    while (!ready.load(std::memory_order_acquire)) {
        std::this_thread::yield();
    }

    // 2. 读取普通数据
    // 因为前面有了 acquire 屏障，这里的 payload_data 一定能看到生产者写入的 42。
    assert(payload_data == 42);
    std::cout << "Consumer read payload: " << payload_data << " (Expected: 42)\n\n";
}

void run() {
    std::cout << "--- 2. Testing memory_order_release & memory_order_acquire ---\n";
    std::jthread c(consumer);
    std::jthread p(producer);
}
}  // namespace ReleaseAcquireTest

// ============================================================================
// 测试 3：第三层级 - 获取并发布 (Acq_Rel)
// ============================================================================
namespace AcqRelTest {
/*
 * 【面试问题】：memory_order_acq_rel 是什么？主要用在哪些地方？
 * 【标准回答】：它是 acquire 和 release 的结合体，既防止前面的指令排到后面，
 * 也防止后面的指令排到前面。
 * 它主要用于“读-改-写 (Read-Modify-Write)”操作（如 exchange, fetch_sub）。
 * 最典型的场景是：1. 实现自旋锁 (Spinlock)；2. shared_ptr 引用计数【减
 * 1】并判断是否销毁对象时。
 */
class SpinLock {
    std::atomic<bool> flag{false};

   public:
    void lock() {
        // 【面试考点】：加锁时必须用 acquire（或更强的模型）。
        // 确保获取锁之后，临界区内的读写操作绝不能跑到获取锁之前执行。
        while (flag.exchange(true, std::memory_order_acquire)) {
            std::this_thread::yield();
        }
    }
    void unlock() {
        // 【面试考点】：解锁时必须用 release（或更强的模型）。
        // 确保临界区内的所有修改，在释放锁之前都已经真正写入内存，对其他线程可见。
        flag.store(false, std::memory_order_release);
    }
};

SpinLock slock;
int shared_resource = 0;

void worker() {
    for (int i = 0; i < 1000; ++i) {
        slock.lock();
        shared_resource++;
        slock.unlock();
    }
}

void run() {
    std::cout << "--- 3. Testing memory_order_acq_rel (SpinLock) ---\n";
    std::vector<std::jthread> threads;
    for (int i = 0; i < 10; ++i) threads.emplace_back(worker);
    std::cout << "SpinLock protected resource: " << shared_resource << " (Expected: 10000)\n\n";
}
}  // namespace AcqRelTest

// ============================================================================
// 测试 4：第四层级 - 顺序一致性 (Seq_Cst)
// ============================================================================
namespace SeqCstTest {
std::atomic<bool> x{false};
std::atomic<bool> y{false};
std::atomic<int> z{0};

/*
 * 【面试问题】：C++ atomic 的默认内存序是什么？它有什么特点和代价？
 * 【标准回答】：默认是 memory_order_seq_cst（顺序一致性）。 seq_cst
 * 的完整拼写是 Sequential Consistency 它是最强、最严格的内存模型。不仅具备
 * release/acquire 的所有同步能力，
 * 还保证了【全局所有线程看到的原子操作顺序都是完全一致的】。
 * 代价是它会在底层触发 CPU 的全屏障指令（Full Memory Barrier），比如 x86 的
 * mfence， 严重阻碍 CPU 乱序执行提升性能的机制，性能开销最大。
 */
void write_x() {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));  // 模拟一些工作
    x.store(true, std::memory_order_seq_cst);
}
void write_y() {
    std::this_thread::sleep_for(std::chrono::milliseconds(55));  // 模拟一些工作
    y.store(true, std::memory_order_seq_cst);
}

void read_x_then_y() {
    while (!x.load(std::memory_order_seq_cst)) {
        std::this_thread::yield();
    }
    if (y.load(std::memory_order_seq_cst)) z.fetch_add(1, std::memory_order_relaxed);
}

void read_y_then_x() {
    while (!y.load(std::memory_order_seq_cst)) {
        std::this_thread::yield();
    }
    if (x.load(std::memory_order_seq_cst)) z.fetch_add(1, std::memory_order_relaxed);
}

void run() {
    std::cout << "--- 4. Testing memory_order_seq_cst ---\n";

    {  // 作用域结束时，所有线程都会自动 join;如果不加这个作用域，z就有可能为0，因为它的读取可能在这些线程还没完成之前就发生了。
       // 另一种办法就是主动调用 join()，但使用 jthread 的好处就是它会自动 join，避免忘记 join 导致的潜在问题。
        std::jthread a(write_x);
        std::jthread b(write_y);
        std::jthread c(read_x_then_y);
        std::jthread d(read_y_then_x);
    }

    // 【面试高频压轴题】：这段代码里的 z 有没有可能等于 0？如果把 seq_cst 换成
    // release/acquire 会怎样？ 【标准回答】：
    // 1. 在 seq_cst 下，z 【绝对不可能】等于 0。因为 seq_cst 保证了 x 和 y
    // 的写入有一个全局绝对先后顺序。
    //    如果 x 先于 y 发生，那么线程 d 一定能看到 x 为 true；如果 y 先于 x
    //    发生，线程 c 一定能看到 y 为 true。
    // 2. 如果换成 release/acquire，z 【有可能】等于 0！
    //    因为 release/acquire 只保证单个变量（x 自身或 y 自身）前后的顺序，不保证
    //    x 和 y 之间的相对顺序。 不同线程由于 CPU 缓存一致性延迟，可能看到 x 和 y
    //    写入的顺序是相反的，导致 if 条件都不成立。
    std::cout << "Waiting for threads to finish... and z:" << z.load() << " (Expected: 1 or 2, strictly != 0)\n";
    assert(z.load() != 0);
    std::cout << "Seq_Cst Result z: " << z.load() << " (Expected: 1 or 2, strictly != 0)\n\n";
}
}  // namespace SeqCstTest

// ============================================================================
// 附赠面试题：被抛弃的 consume
// ============================================================================
/*
 * 【面试问题（加分题）】：你能说说 memory_order_consume 吗？
 * 【标准回答】：consume 的本意是实现比 acquire
 * 更轻量级的同步，它只同步有“数据依赖”的变量。
 * 例如，通过指针传递数据时，只同步指针所指的具体内容。
 * 但因为在编译器层面去精确追踪“数据依赖”太过困难且容易出错，目前的 C++
 * 主流编译器 （GCC/Clang）基本上都直接把 consume 降级当成 acquire 来处理了。
 * C++17
 * 标准委员会也正式建议开发者不再使用（discourage）它。所以在实际项目中，直接用
 * acquire 即可。
 */

// ============================================================================
// Main 函数
// ============================================================================
int main() {
    std::cout << "Starting C++ Memory Order Interview Demo...\n\n";

    RelaxedTest::run();
    ReleaseAcquireTest::run();
    AcqRelTest::run();
    SeqCstTest::run();

    std::cout << "All tests executed and passed! You are ready for the interview!\n";
    return 0;
}