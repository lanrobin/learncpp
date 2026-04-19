#include <iostream>
using namespace std;

class DelegateConstructor {
public:
    DelegateConstructor() : DelegateConstructor(0) {
        cout << "Default constructor called" << endl;
    }

    DelegateConstructor(int x) : DelegateConstructor(x, 0) {
        cout << "Single parameter constructor called with x = " << x << endl;
    }

    DelegateConstructor(int x, int y) {
        cout << "Two parameter constructor called with x = " << x << " and y = " << y << endl;
    }
};

int main() {
    DelegateConstructor obj1; // Calls default constructor
    DelegateConstructor obj2(5); // Calls single parameter constructor
    DelegateConstructor obj3(5, 10); // Calls two parameter constructor

    return 0;
}