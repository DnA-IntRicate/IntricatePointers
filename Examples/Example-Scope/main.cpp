#include <iostream>
#include <IntricatePointers/IntricatePointers.hpp>
using namespace Intricate;


class ExampleClass
{
public:
    constexpr ExampleClass(float f1, float f2, int64_t i1) noexcept : m_F1(f1), m_F2(f2), m_I1(i1) { };
    ExampleClass() = default;

    ~ExampleClass() noexcept
    {
        std::cout << "ExampleClass .dtor called on " << this << '\n';
    }

    constexpr float GetF1() const noexcept { return m_F1; }
    constexpr float GetF2() const noexcept { return m_F2; }
    constexpr int64_t GetI1() const noexcept { return m_I1; }

private:
    float m_F1;
    float m_F2;
    int64_t m_I1;
};

class BaseClass
{
public:
    constexpr BaseClass(int number) noexcept : m_Number(number) { };
    BaseClass() = default;

    // This destructor must be marked virtual to allow the destructors of any child classes to also be called when this object is free'd.
    virtual ~BaseClass() noexcept
    {
        std::cout << "BaseClass .dtor called on " << this << '\n';
    }

    virtual void DoSomething() const noexcept = 0;
    virtual void DoSomethingElse() const noexcept = 0;

    constexpr int GetNumber() const noexcept { return m_Number; }

protected:
    int m_Number;
};

class DerivedClass : public BaseClass
{
public:
    constexpr DerivedClass(int number) noexcept : BaseClass(number) { };

    // This destructor will always be called before BaseClass's destructor.
    virtual ~DerivedClass() noexcept
    {
        std::cout << "DerivedClass .dtor called on " << this << '\n';
    }

    virtual void DoSomething() const noexcept override
    {
        std::cout << "DerivedClass::DoSomething() called on " << this << '\n';
    }

    virtual void DoSomethingElse() const noexcept override
    {
        std::cout << "DerivedClass::DoSomethingElse() called on " << this << '\n';
    }
};

int main(int argc, char** argv)
{
    std::cout << "----------------------------------------------------------------\n";
    std::cout << "Example-Scope\n";
    std::cout << "----------------------------------------------------------------\n\n";

    // Creates a unique scoped pointer to a newly heap-allocated and constructed instance of 'ExampleClass'
    Scope<ExampleClass> constructedScope = CreateScope<ExampleClass>(23.5f, 19.2f, INT64_MAX);
    std::cout << "constructedScope address: " << constructedScope << '\n';

    // Copying a scope cannot be done since it's unique
    // Scope<ExampleClass> newConstructedScope = constructedScope;

    // Instead it must be moved to reassign it.
    Scope<ExampleClass> newConstructedScope = std::move(constructedScope);
    std::cout << "newConstructedScope address after move: " << newConstructedScope << '\n';
    std::cout << "constructedScope address after move: " << constructedScope << '\n';   // Intentional use-after-move
    
    // Access the data of newConstructedScope
    std::cout << "newConstructedScope F1: " << newConstructedScope->GetF1() << '\n';
    std::cout << "newConstructedScope F2: " << newConstructedScope->GetF2() << '\n';
    std::cout << "newConstructedScope I1: " << newConstructedScope->GetI1() << '\n';

    // Free newConstructedScope
    newConstructedScope = nullptr;  // Alternatively: newConstructedScope.Reset();

    // Create a new scope of type 'BaseClass' that actually points to a 'DerivedClass'
    Scope<BaseClass> baseClassScope = CreateScope<DerivedClass>(21);
    baseClassScope->DoSomething();
    baseClassScope->DoSomethingElse();
    std::cout << "baseClassScope Number: " << baseClassScope->GetNumber() << '\n';
    baseClassScope.Reset();

    // When 'scopedScope' falls out of scope, it will be free'd.
    {
        auto scopedScope = CreateScope<ExampleClass>(22.0f, -65.0f, INT64_MIN);
        std::cout << "scopedScope address: " << scopedScope << '\n';
        std::cout << "scopedScope F1: " << scopedScope->GetF1() << '\n';
        std::cout << "scopedScope F2: " << scopedScope->GetF2() << '\n';
        std::cout << "scopedScope I1: " << scopedScope->GetI1() << '\n';
    }

    // Create a scope to an int and print the int by dereferencing the smart pointer.
    Scope<int> intScope = CreateScope<int>(11);
    std::cout << "intScope: " << *intScope << '\n';
    std::cout << "intScope address: " << intScope << '\n';

    // Release ownership of the pointer
    int* intPtr = intScope.Release();
    std::cout << "intScope address after release: " << intScope << '\n';
    std::cout << "intPtr: " << *intPtr << '\n';
    std::cout << "intPtr address: " << intPtr << '\n';

    // Since intScope was allocated with new and we released its ownership of the pointer, we have to manually clean it up by using delete.
    delete intPtr;

    std::cin.get();
    return 0;
}
