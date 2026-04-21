#include <iostream>
#include <string>
#include <vector>

// =====================================================================
// The Actors (Different classes)
// =====================================================================

// Actor 1: A real bird
class RealDuck {
public:
    // It can quack
    void quack() const {
        std::cout << "RealDuck: Quack! Quack!\n";
    }
};

// Actor 2: A human who mimics a duck
class Human {
public:
    // It also has a quack method, but it is NOT a bird!
    void quack() const {
        std::cout << "Human: I am mimicking a duck... Quack!\n";
    }
};

// Actor 3: A dog
class Dog {
public:
    // It barks, it cannot quack
    void bark() const {
        std::cout << "Dog: Woof!\n";
    }
};


// =====================================================================
// Paradigm 1: Nominal Typing (The OOP Way)
// Focuses on inheritance. "Are you officially a duck?"
// =====================================================================

// An official ID card for ducks
class IDuck {
public:
    virtual void quack() const = 0;
    virtual ~IDuck() = default;
};

// OfficialDuck explicitly inherits from IDuck
class OfficialDuck : public IDuck {
public:
    void quack() const override {
        std::cout << "OfficialDuck: Quack!\n";
    }
};

// This function strictly requires the IDuck base class
void nominal_test(const IDuck& duck) {
    duck.quack();
}


// =====================================================================
// Paradigm 2: Duck Typing (C++98/11 Templates)
// Focuses on behavior. "I don't care what you are, just quack!"
// =====================================================================

// The compiler blindly trusts T has a quack() method until instantiation
template <typename T>
void duck_typing_test(const T& entity) {
    // If 'entity' doesn't have quack(), it fails with a horrible error msg
    entity.quack(); 
}


// =====================================================================
// Paradigm 3: Structural Typing (C++20 Concepts)
// The safe Duck Typing. "Let me check your abilities before compiling."
// =====================================================================

// Define a structural requirement (Concept)
template <typename T>
concept Quackable = requires(T a) {
    a.quack(); // The type MUST have a quack() method
};

// The function is protected by the Concept
void structural_test(const Quackable auto& entity) {
    entity.quack();
}

// =====================================================================
// Main function to test all paradigms
// =====================================================================
int main() {
    OfficialDuck o_duck;
    RealDuck r_duck;
    Human human;
    Dog dog;

    std::cout << "--- 1. Nominal Typing Test ---\n";
    nominal_test(o_duck); 
    // nominal_test(r_duck); // ERROR! RealDuck doesn't inherit from IDuck
    // nominal_test(human);  // ERROR! Human doesn't inherit from IDuck


    std::cout << "\n--- 2. Duck Typing Test (Templates) ---\n";
    duck_typing_test(r_duck); // OK! RealDuck has quack()
    duck_typing_test(human);  // OK! Human has quack()! The compiler doesn't care it's a human.
    // duck_typing_test(dog); // ERROR! Dog has no quack(). Compilation fails with messy SFINAE logs.


    std::cout << "\n--- 3. Structural Typing Test (C++20 Concepts) ---\n";
    structural_test(r_duck); // OK! Matches the Quackable concept.
    structural_test(human);  // OK! Matches the Quackable concept.
    // structural_test(dog); // ERROR! Compilation fails cleanly: "constraints not satisfied".

    return 0;
}