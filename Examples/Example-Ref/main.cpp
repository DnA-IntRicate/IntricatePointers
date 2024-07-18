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
        std::cout << "Destructor being called on " << this << "\n";
    }

    constexpr float GetF1() const noexcept { return m_F1; }
    constexpr float GetF2() const noexcept { return m_F2; }
    constexpr int64_t GetI1() const noexcept { return m_F2; }

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

    virtual ~BaseClass() noexcept
    {
        std::cout << "BaseClass .dtor called on " << this << "\n";
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

    ~DerivedClass() noexcept override
    {
        std::cout << "DerivedClass .dtor called on " << this << "\n";
    }

    virtual void DoSomething() const noexcept override
    {
        std::cout << "DerivedClass::DoSomething() called on " << this << "\n";
    }

    virtual void DoSomethingElse() const noexcept override
    {
        std::cout << "DerivedClass::DoSomethingElse() called on " << this << "\n";
    }
};

int main(int argc, char** argv)
{
    // Creates a reference-counted pointer to a newly heap-allocated and constructed instance of 'ExampleClass'
    Ref<ExampleClass> constructedRef = CreateRef<ExampleClass>(23.5f, 19.2f, INT64_MAX);

    // Retrieve the reference-count (use_count) of the pointer
    std::cout << "constructedRef ref-count: " << constructedRef.RefCount() << "\n";

    // Increment the ref count by assigning it to a new object
    Ref<ExampleClass> constructedRefCopy = constructedRef;
    std::cout << "constructedRef ref-count after new assignment: " << constructedRef.RefCount() << "\n";

    // Access the data of constructedRef
    std::cout << "constructedRef F1: " << constructedRef->GetF1() << "\n";
    std::cout << "constructedRef F2: " << constructedRef->GetF2() << "\n";
    std::cout << "constructedRef I1: " << constructedRef->GetI1() << "\n";

    // When releasing the reference held by constructedRefCopy, the underlying data pointed to by constructedRef does not
    // get freed yet since the reference-count hasn't yet hit 0
    constructedRefCopy = nullptr;
    std::cout << "constructedRef ref-count after release: " << constructedRef.RefCount() << "\n";

    // Once we release the reference held by constructedRef, the reference-count will hit 0 and the data will be freed
    constructedRef = nullptr;

    // Create a new ref of type 'BaseClass' that actually points to a 'DerivedClass'
    Ref<BaseClass> baseClassRef = CreateRef<DerivedClass>(21);
    baseClassRef->DoSomething();
    baseClassRef->DoSomethingElse();
    std::cout << "baseClassRef m_Number: " << baseClassRef->GetNumber() << "\n";

    std::cin.get();
    return 0;
}
