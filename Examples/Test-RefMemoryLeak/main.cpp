#include <iostream>
#include <IntricatePointers/IntricatePointers.hpp>
using namespace Intricate;


class MemLeakTest
{
public:
    constexpr MemLeakTest(size_t idx) noexcept : m_Index(idx) { };
    constexpr MemLeakTest() noexcept = default;

    ~MemLeakTest() noexcept
    {
        std::cout << "Destructing... #" << m_Index << '\n';
    }

    constexpr size_t GetIndex() const noexcept { return m_Index; }

private:
    size_t m_Index = 0;
};

int main(int argc, char** argv)
{
    std::cout << "----------------------------------------------------------------\n";
    std::cout << "Test-RefMemoryLeak\n";
    std::cout << "----------------------------------------------------------------\n\n";

    std::cout << "Press Enter to start memory leak test\n";
    std::cin.get();
    
    constexpr size_t MAX_ITERS = UINT64_MAX;
    for (size_t i = 0; i < MAX_ITERS; ++i)
    {
        std::cout << "Constructing #" << i << '\n';
        Ref<MemLeakTest> ptr = CreateRef<MemLeakTest>(i);
        (void)ptr->GetIndex();  // Just accessing this for the sake of accessing it.
    }

    std::cin.get();
    return 0;
}
