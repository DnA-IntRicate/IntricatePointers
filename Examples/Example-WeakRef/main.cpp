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
    std::cout << "Example-WeakRef\n";
    std::cout << "----------------------------------------------------------------\n\n";

    // Creates a reference counted pointer to a newly heap-allocated and constructed instance of 'ExampleClass'
    Ref<ExampleClass> constructedRef = CreateRef<ExampleClass>(23.5f, 19.2f, INT64_MAX);

    // Retrieve the reference count (use_count) of the pointer
    std::cout << "constructedRef ref-count: " << constructedRef.RefCount() << '\n';

    // Create a weak reference to 'constructedRef' by copy-assignment (this does not increase the ref count)
    WeakRef<ExampleClass> constructedWeakRef = constructedRef;
    std::cout << "constructedWeakRef ref-count after weak assignment: " << constructedWeakRef.RefCount() << '\n';

    // Access the data of constructedRef
    std::cout << "constructedRef F1: " << constructedRef->GetF1() << '\n';
    std::cout << "constructedRef F2: " << constructedRef->GetF2() << '\n';
    std::cout << "constructedRef I1: " << constructedRef->GetI1() << '\n';

    // Release constructedRef. The reference count will now be 0 despite the weak reference causing the data to be free'd
    constructedRef = nullptr;
    std::cout << "constructedRef ref-count after release: " << constructedRef.RefCount() << '\n';

    // We will not be able to access any of the data through the weak reference since the reference has 'expired'
    std::cout << "constructedWeakRef count: " << constructedWeakRef.RefCount() << '\n';
    if (constructedWeakRef.Expired())
        std::cout << "constructedWeakRef has expired!\n";

    // This only sets the underlying pointer to nullptr since WeakRef is non-owning.
    constructedWeakRef = nullptr;
    
    // Create a new ref of type 'BaseClass' that actually points to a 'DerivedClass'
    Ref<BaseClass> baseClassRef = CreateRef<DerivedClass>(21);
    baseClassRef->DoSomething();
    baseClassRef->DoSomethingElse();
    std::cout << "baseClassRef Number: " << baseClassRef->GetNumber() << '\n';

    // Create a weak reference to 'baseClassRef'
    WeakRef<BaseClass> baseClassWeakRef = baseClassRef;

    // Access the weak ref by locking it.
    // This will increment the reference count while the locked ref is in scope to ensure that the resources aren't deleted
    if (Ref<BaseClass> lockedRef = baseClassWeakRef.Lock())
    {
        std::cout << "baseClassWeakRef ref-count after lock: " << baseClassWeakRef.RefCount() << '\n';

        // Since the weak reference is now locked, resetting 'baseClassRef' will not release any resources
        baseClassRef.Reset();
        std::cout << "baseClassWeakRef ref-count in lock after reset: " << baseClassWeakRef.RefCount() << '\n';

        // We can now access the weak reference through the locked reference
        lockedRef->DoSomething();
        lockedRef->DoSomethingElse();
        std::cout << "lockedRef Number: " << lockedRef->GetNumber() << '\n';
    } // The locked reference will fall out of scope here decrementing the reference count to 0 causing the resources to be released

    // The 'baseClassWeakRef' should now be expired, if we try to lock an expired WeakRef, it will return nullptr
    if (Ref<BaseClass> lockedRef = baseClassWeakRef.Lock())
    {
        // This will never be executed
    }
    else
    {
        std::cout << "Failed to lock baseClassWeakRef since the reference has expired!\n";
        baseClassWeakRef = nullptr;
    }

    // Reset the pointer and set the internal weak reference count to 0
    baseClassWeakRef = nullptr;

    std::cin.get();
    return 0;
}
