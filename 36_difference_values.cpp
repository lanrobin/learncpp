/**
 1. 三种基础值（Primary Categories）
lvalue（左值）：有身份，不能被移动。

大白话： 稳稳当当存在内存里的变量，有名字，你可以取它的地址。因为别人可能还在用它，所以你不能随便偷它的资源。

prvalue（纯右值）：没身份，能被移动。

大白话： 临时的、连名字都没有的计算结果或字面量（比如 42，a + b）。用完这一行代码它就灰飞烟灭了，所以你可以随便偷它的内部资源。

xvalue（将亡值，eXpiring value）：有身份，也能被移动。（C++11 诞生的核心）

大白话： 它本来是个左值（有名字有地址），但程序员通过某种手段（最常见的就是 std::move）明确表示：“这个变量我马上不用了，它的命快到期了，你可以把它的资源偷走！”

2. 两种组合值（Mixed Categories）
为了方便编译器写规则，C++ 把上面三种基础值两两组合，得到了两个“大类”：

glvalue（泛左值，Generalized lvalue）= lvalue + xvalue

所有**有身份（能取地址）**的值的总称。

rvalue（右值）= prvalue + xvalue

所有**能被移动（能偷资源）**的值的总称。
 */

 #include <iostream>
#include <string>
#include <utility>
#include <cstring>

class Resource {
public:
    char* data; // 为了最直观地演示，我们使用裸指针指向堆内存

    // ====================================================
    // 0. 普通构造函数
    // ====================================================
    Resource(const char* str) {
        if (str) {
            data = new char[strlen(str) + 1]; // 申请一块堆内存
            strcpy(data, str);                // 填入数据
        } else {
            data = nullptr;
        }
        std::cout << "[普通构造] 为 '" << (data ? data : "null") << "' 申请了新内存: " << (void*)data << "\n";
    }

    // ====================================================
    // 1. 拷贝构造函数 (Copy Constructor) - 老实人的做法
    // ====================================================
    Resource(const Resource& other) {
        if (other.data) {
            // 【深拷贝的核心】：自己掏钱，重新申请一块【全新】的内存
            data = new char[strlen(other.data) + 1]; 
            // 把别人的数据，一个字节一个字节地抄过来
            strcpy(data, other.data);                
        } else {
            data = nullptr;
        }
        std::cout << "[拷贝构造] 深拷贝 '" << (other.data ? other.data : "null") 
                  << "' 到新内存: " << (void*)data << "\n";
    }

    // ====================================================
    // 2. 拷贝赋值运算符 (Copy Assignment)
    // ====================================================
    Resource& operator=(const Resource& other) {
        if (this != &other) { // 防止自我赋值 (res = res)
            delete[] data;    // 先清理自己原有的内存

            if (other.data) {
                // 再次执行老实人的深拷贝流程
                data = new char[strlen(other.data) + 1];
                strcpy(data, other.data);
            } else {
                data = nullptr;
            }
            std::cout << "[拷贝赋值] 销毁原内存，深拷贝数据到新内存: " << (void*)data << "\n";
        }
        return *this;
    }

    // ====================================================
    // 3. 移动构造函数 (Move Constructor) - 偷窃的核心！
    // ====================================================
    Resource(Resource&& other) noexcept {
        // 【偷窃动作 1】：直接把对方指向堆内存的指针抢过来！不申请新内存！
        data = other.data; 
        
        // 【偷窃动作 2】：销毁证据！把对方的指针设为空 (nullptr)。
        // 这一步极其关键，否则析构时两个人都会 delete 同一块内存，导致程序崩溃。
        other.data = nullptr; 
        
        std::cout << "[移动构造] 直接霸占指针 " << (void*)data << "，并将原对象掏空置为 null！\n";
    }

    // ====================================================
    // 4. 移动赋值运算符 (Move Assignment)
    // ====================================================
    Resource& operator=(Resource&& other) noexcept {
        if (this != &other) {
            delete[] data; // 把自己手头现有的破烂扔掉
            
            // 再次执行偷窃
            data = other.data;
            other.data = nullptr;
            
            std::cout << "[移动赋值] 销毁自身原内存，霸占指针 " << (void*)data << "，并将原对象掏空！\n";
        }
        return *this;
    }

    // ====================================================
    // 5. 析构函数 (Destructor)
    // ====================================================
    ~Resource() {
        if (data) {
            std::cout << "[析构函数] 释放内存: " << (void*)data << " ('" << data << "')\n";
            delete[] data;
        } else {
            std::cout << "[析构函数] 这是一个被掏空的对象，安全退出，无需释放内存。\n";
        }
    }
};

// ====================================================
// 辅助函数：返回一个纯右值
// ====================================================
Resource makeTemporaryResource() {
    return Resource("Temp_Data"); // 函数内创建，按值返回，产生 prvalue
}
// 返回左值引用 (lvalue reference) 的函数
Resource& getGlobalResource() {
    static Resource global("Global Data");
    return global;
}

int main() {
    // ====================================================
    // 1. 左值 (lvalue)：有名字，有确定的内存地址
    // ====================================================
    std::cout << "========== 场景 1：左值 (lvalue) 与深拷贝 ==========\n";
    int a = 10;           // 'a' 是左值
    int* p = &a;          // 'a' 可以被取地址
    const int b = 20;     // 'b' 是 const 左值（不能改，但依然是左值）

    // res1 是一个【左值 (lvalue)】：它在内存中有确定的地址，有名字。
    Resource res1("Data_A"); 
    
    // res1 是左值，编译器认为它可能以后还要用，为了安全，触发【拷贝构造】（深拷贝）
    Resource res2(res1); 
    
    std::cout << "  -> res1 的指针: " << (void*)res1.data << "\n";
    std::cout << "  -> res2 的指针: " << (void*)res2.data << "\n\n";

    getGlobalResource();    // 函数返回左值引用，所以这个函数调用表达式也是左值！
    
    ++a; // 前置递增返回的是改变后的变量本身，所以 '++a' 是左值


    // ====================================================
    // 2. 纯右值 (prvalue)：没有名字的临时对象、字面量
    // ====================================================
    std::cout << "========== 场景 2：纯右值 (prvalue) 与直接移动 ==========\n";
    42;             // 42 是纯右值（除了字符串字面量如 "hello"，它是左值）
    a + 5;          // 'a + 5' 的计算结果存在临时的寄存器里，是纯右值
    
    // makeTemporaryResource() 的返回值是一个临时的、没名字的对象。
    // 这属于【纯右值 (prvalue)】。
    // 既然是临时的，用完就扔，编译器直接触发【移动构造】，把它的内部资源偷给 res3。
    // (注：现代 C++ 甚至会做 RVO 优化直接在 res3 的位置构造，但逻辑上它匹配移动构造)
    Resource res3(makeTemporaryResource());
    std::cout << "  -> res3 拿到了临时对象的底层指针: " << (void*)res3.data << "\n\n";
    
    a++; // 后置递增返回的是 a 改变之前的“临时副本”，所以 'a++' 是纯右值！
         // （这也是为什么 C++ 里提倡用 ++a 而不是 a++ 的底层原因，a++ 产生了临时纯右值）


    // ====================================================
    // 3. 将亡值 (xvalue)：本是有名字的左值，被强行标记为即将消亡
    // ====================================================
    std::cout << "========== 场景 3：将亡值 (xvalue) 与人为偷窃 ==========\n";
    Resource res4("Data_B"); // res4 本是个左值
   
    // std::move(res4) 并不移动任何东西！
    // 它只是把 res4 强行转换成了一个【将亡值 (xvalue)】。
    // 将亡值告诉编译器：“这个左值我不要了，你可以把它当右值来用。”
    // 因为传进去的是将亡值，触发了【移动构造】，res5 成功偷走了 res4 的钥匙。
    Resource res5(std::move(res4)); 
    
    std::cout << "  -> res4 被掏空后的指针: " << (void*)res4.data << " (空)\n";
    std::cout << "  -> res5 抢到的新指针: " << (void*)res5.data << "\n\n";
    
    std::cout << "========== 场景 4：面试终极陷阱（右值引用变量的真面目） ==========\n";
    // r_ref 是一个绑定到 res5 的右值引用。
    Resource&& r_ref = std::move(res5); 

    std::cout << "r_ref 的指针: " << &r_ref << "\n res5 的指针: " << &res5 << "\n指向同一个地址。\n\n";
    
    // 【陷阱】：r_ref 这个表达式本身，到底是左值还是右值？
    // 答案是：【左值 (lvalue)】！因为 r_ref 有名字，你能对它取地址 (&r_ref)。
    // 既然它是左值，把它传给 res6，触发的依然是老老实实的【拷贝构造】！(没有偷窃发生)
    std::cout << "[尝试传入 r_ref...]\n";
    Resource res6(r_ref); 
    
    // 如果你真想偷 r_ref 指向的东西，你必须再次把它转为将亡值 (xvalue)！
    std::cout << "[尝试传入 std::move(r_ref)...]\n";
    Resource res7(std::move(r_ref)); // 此时触发【移动构造】！
    
    std::cout << "\n========== 场景 5：程序结束，大面积析构开始 ==========\n";
    // 总结：
    // 【泛左值 (glvalue)】 = 左值 (lvalue) + 将亡值 (xvalue)  --> 能在内存找到具体位置的。
    // 【右值 (rvalue)】 = 纯右值 (prvalue) + 将亡值 (xvalue)  --> 能触发“偷窃(Move)”的。
    return 0;
}