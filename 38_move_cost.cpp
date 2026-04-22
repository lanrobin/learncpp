#include <utility>

// 我们不用 main 函数，直接写一个普通函数
// 这样可以避免主函数产生一堆无关的初始化代码
int my_test_function() {
    int x = 42;
    int&& y = std::move(x); 
    return y;
}

int main() 
{
    my_test_function();
}
/*

g++ -O0 -S -masm=intel 38_move_cost.cpp -o unoptimized_38_move_cost.s

g++ -O2 -S -masm=intel 38_move_cost.cpp -o optimized_38_move_cost.s

cat unoptimized_38_move_cost.s

cat optimized_38_move_cost.s

or you can go to https://godbolt.org/ to see the generated assembly online.

*/