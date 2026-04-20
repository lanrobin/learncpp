#include<iostream>
#include<typeinfo>
 
template<typename T>
struct A
{
    int num;
    A()
    {
        std::cout << "A Construct" << std::endl;
        std::cout << "template typename is: " << typeid (T).name() << std::endl;
    }
};
//此处的T可省略，X代表模板类型，T和X前的typename可替换成class

#if __cplusplus >= 201703L
template<template<typename> typename X>
#else
template<template<class T> class X>
#endif
struct B
{
    X<double> e;
    B() { 
        
        std::cout << "B Construct" << std::endl;
        std::cout << "template template typename is: " << typeid (X<double>).name() << std::endl;
        //std::cout << "template template typename is: " << typeid (T).name() << std::endl;
    }
};

#if __cplusplus >= 201703L
template<template<typename> typename X>
#else
template<template<class> class X>
#endif
struct C
{
    X<int> c;
    C() {
        std::cout << "C Construct" << std::endl;
        std::cout << "template typename is: " << typeid (X<int>).name()
                    << std::endl;
    }
};


template<typename U, typename V>
struct DParamter {
    U u;
    V v;
    DParamter() {
        std::cout << "DParamter Construct" << std::endl;
        std::cout << "template typename is: " << typeid (U).name() << ", " << typeid (V).name() << std::endl;
    }
};

#if __cplusplus >= 201703L
template<template<typename, typename> typename X>
#else
template<template<class, class> class X>
#endif
struct D
{
    X<int, double> d;
    D() {
        std::cout << "D Construct" << std::endl;
        std::cout << "template typename is: " << typeid (X<int, double>).name()
                    << std::endl;
    }
};

#if __cplusplus >= 201703L
// 这里的 typename... 表示：X 可以是一个带任意数量参数的模板！
template<typename... T>
#else
template<class... T>
#endif
struct SuperDParameter {
    SuperDParameter() {
        std::cout << "SuperDParameter Construct" << std::endl;
        std::cout << "template typename is: ";
        ((std::cout << typeid (T).name() << ", "), ...);
        std::cout << std::endl;
    }
};
 

#if __cplusplus >= 201703L
// 这里的 typename... 表示：X 可以是一个带任意数量参数的模板！
template<template<typename...> typename X>
#else
template<template<class...> class X>
#endif
struct SuperD
{
    // 在这里面，你想传几个就传几个！
    X<int> d1;
    X<int, double> d2;
    X<int, double, char, float> d3;

    SuperD() {
        std::cout << "SuperD Construct" << std::endl;
        std::cout << "template typename is: " << typeid (X<int>).name() << ", "
                    << typeid (X<int, double>).name() << ", "
                    << typeid (X<int, double, char, float>).name() << std::endl;
    }
};

int main()
{
    A<B<A>> a;
    std::cout << "***************************" << std::endl;
    B<A> b;
    std::cout << "***************************" << std::endl;
    C<A> c;
    std::cout << "***************************" << std::endl;
    D<DParamter> d;
    std::cout << "***************************" << std::endl;
    SuperD<SuperDParameter> superd;
    return 0;
}