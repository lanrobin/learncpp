#include<iostream>

template<typename T, T... ints>
int unary_left_fold(std::integer_sequence<T, ints...> int_seq)
{
    int result = (... + ints);
    std::cout << "The sequence of size " << int_seq.size() << ", " << result <<"= ";
    (..., (std::cout << ints << '+'));
    std::cout << std::endl;
    return result;
}

template<typename T, T... ints>
int binary_left_fold(std::integer_sequence<T, ints...> int_seq)
{
    int result = (0 + ... + ints);
    std::cout << "The sequence of size " << int_seq.size() << ", " << result <<"=0+";
    (..., (std::cout << ints << '+'));
    std::cout << std::endl;
    return result;
}

template<typename T, T... ints>
int unary_right_fold(std::integer_sequence<T, ints...> int_seq)
{
    int result = (ints + ...);
    std::cout << "The sequence of size " << int_seq.size() << ", " << result <<"=";
    (..., (std::cout << ints << '+'));
    std::cout << std::endl;
    return result;
}

template<typename T, T... ints>
int binary_right_fold(std::integer_sequence<T, ints...> int_seq)
{
    int result = (ints + ... + 0);
    std::cout << "The sequence of size " << int_seq.size() << ", " << result <<"=";
    (..., (std::cout << ints << '+'));
    std::cout << "0" << std::endl;
    return result;
}

int main() {
    std::cout << unary_left_fold(std::integer_sequence<int, 1, 2, 3>{}) << std::endl;
    std::cout << binary_left_fold(std::integer_sequence<int, 1, 2, 3>{}) << std::endl;
    std::cout << unary_right_fold(std::integer_sequence<int, 1, 2, 3>{}) << std::endl;
    std::cout << binary_right_fold(std::integer_sequence<int, 1, 2, 3>{}) << std::endl;
    return 0;
}