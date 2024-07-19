#pragma once
#include <memory>
#include <atomic>


namespace Intricate
{
    template<typename _Ty>
    class Scope
    {
    public:
        constexpr explicit Scope(_Ty* ptr) noexcept : m_Ptr(ptr) { };

        template<typename _Ty2, std::enable_if_t<std::is_base_of_v<_Ty, _Ty2>, int> = 0>
        constexpr explicit Scope(_Ty2* ptr) noexcept { m_Ptr = static_cast<_Ty*>(ptr); }

        template<typename _Ty2, std::enable_if_t<std::is_base_of_v<_Ty, _Ty2>, int> = 0>
        constexpr Scope(Scope<_Ty2>&& other) noexcept { m_Ptr = static_cast<_Ty*>(other.Release()); }

        constexpr Scope(Scope<_Ty>&& other) noexcept : m_Ptr(other.Release()) { };

        Scope(Scope<_Ty>&) = delete;
        Scope(const Scope<_Ty>&) = delete;
        constexpr Scope(std::nullptr_t) noexcept : m_Ptr(nullptr) { };
        Scope() = default;
        constexpr ~Scope() noexcept { Delete(); }

        constexpr void Swap(Scope<_Ty>& other) noexcept
        {
            if (this != &other)
                std::swap(m_Ptr, other.m_Ptr);
        }

        constexpr void Reset(_Ty* newPtr = nullptr) noexcept
        {
            Delete();
            m_Ptr = newPtr;
        }

        constexpr _Ty* Release() noexcept
        {
            _Ty* releasedPtr = m_Ptr;
            m_Ptr = nullptr;

            return releasedPtr;
        }

        constexpr _Ty* Raw() const noexcept { return m_Ptr; }

        constexpr bool Valid() const noexcept { return Raw() != nullptr; }
        constexpr explicit operator bool() const noexcept { return Valid(); }

        Scope<_Ty>& operator=(Scope<_Ty>&) = delete;
        Scope<_Ty>& operator=(const Scope<_Ty>&) = delete;

        constexpr Scope<_Ty>& operator=(Scope<_Ty>&& other) noexcept
        {
            Reset(other.Release());

            return *this;
        }

        constexpr Scope<_Ty>& operator=(std::nullptr_t) noexcept
        {
            Reset();

            return *this;
        }

        constexpr _Ty* operator->() const noexcept { return Raw(); }
        constexpr _Ty& operator*() const noexcept { return *Raw(); }

    private:
        void Delete() noexcept
        {
            if (m_Ptr)
                delete m_Ptr;
        }

    private:
        template<typename _Ty2>
        friend class Scope;

    private:
        _Ty* m_Ptr = nullptr;
    };

    template<typename _Ty, typename... _Args, std::enable_if_t<std::negation_v<std::is_array<_Ty>>, int> = 0>
    constexpr inline static Scope<_Ty> CreateScope(_Args&&... args) noexcept
    {
        return Scope<_Ty>(new _Ty(std::forward<_Args>(args)...));
    }

    template<typename _WantedType, typename _ScopeType>
    constexpr inline static _WantedType* GetScopeBaseTypePtr(const Scope<_ScopeType>& scope) noexcept
    {
        return static_cast<_WantedType*>(scope.Raw());
    }

    template<typename _Elem, typename _Traits, typename _Ty>
    constexpr inline static std::basic_ostream<_Elem, _Traits>& operator<<(std::basic_ostream<_Elem, _Traits>& ostream, const Scope<_Ty>& ptr)
    {
        return ostream << ptr.Raw();
    }

    // This forward declaration is required by Ref
    template<typename _Ty>
    class WeakRef;

    template<typename _Ty>
    class Ref
    {
    public:
        constexpr explicit Ref(_Ty* ptr) noexcept : m_Ptr(ptr) { m_RefCount = (ptr) ? new std::atomic_uint(1) : nullptr; }

        template<typename _Ty2, std::enable_if_t<std::is_base_of_v<_Ty, _Ty2>, int> = 0>
        constexpr explicit Ref(_Ty2* ptr = nullptr) noexcept : m_Ptr(ptr) { m_RefCount = (ptr) ? new std::atomic_uint(1) : nullptr; }

        template<typename _Ty2, std::enable_if_t<std::is_base_of_v<_Ty, _Ty2>, int> = 0>
        constexpr Ref(const Ref<_Ty2>& other) noexcept : m_Ptr(other.m_Ptr), m_RefCount(other.m_RefCount) { IncRef(); }

        Ref(const Ref<_Ty>& other) noexcept
        {
            if (this != &other)
            {
                m_Ptr = other.m_Ptr;
                m_RefCount = other.m_RefCount;

                IncRef();
            }
        }

        template<typename _Ty2, std::enable_if_t<std::is_base_of_v<_Ty, _Ty2>, int> = 0>
        constexpr Ref(Ref<_Ty2>&& other) noexcept : m_Ptr(other.m_Ptr), m_RefCount(other.m_RefCount)
        {
            other.m_Ptr = nullptr;
            other.m_RefCount = nullptr;
        }

        constexpr Ref(Ref<_Ty>&& other) noexcept : m_Ptr(other.m_Ptr), m_RefCount(other.m_RefCount)
        {
            other.m_Ptr = nullptr;
            other.m_RefCount = nullptr;
        }

        constexpr Ref(std::nullptr_t) noexcept : m_Ptr(nullptr), m_RefCount(nullptr) { };
        Ref() = default;
        constexpr ~Ref() noexcept { DecRef(); }

        constexpr void Swap(Ref<_Ty>& other) noexcept
        {
            if (this != &other)
            {
                std::swap(m_Ptr, other.m_Ptr);
                std::swap(m_RefCount, other.m_RefCount);
            }
        }

        constexpr void Reset() noexcept
        {
            Ref<_Ty>(nullptr).Swap(*this);
        }

        uint32_t RefCount() const noexcept { return m_RefCount ? m_RefCount->load() : 0; }
        bool Unique() const noexcept { return RefCount() == 1; }

        constexpr _Ty* Raw() const noexcept { return m_Ptr; }
        constexpr bool Valid() const noexcept { return Raw() != nullptr; }

        constexpr explicit operator bool() const noexcept { return Valid(); }
        constexpr operator Ref<const _Ty>() const noexcept { return Ref<const _Ty>{ m_Ptr, m_RefCount }; }

        Ref<_Ty>& operator=(const Ref<_Ty>& other) noexcept
        {
            Ref<_Ty>(other).Swap(*this);
            return *this;
        }

        template<typename _Ty2, std::enable_if_t<std::is_base_of_v<_Ty, _Ty2>, int> = 0>
        constexpr Ref<_Ty>& operator=(const Ref<_Ty2>& other) noexcept
        {
            Ref<_Ty>(other).Swap(*this);
            return *this;
        }

        Ref<_Ty>& operator=(Ref<_Ty>&& other) noexcept
        {
            Ref<_Ty>(std::move(other)).Swap(*this);
            return *this;
        }

        template<typename _Ty2, std::enable_if_t<std::is_base_of_v<_Ty, _Ty2>, int> = 0>
        constexpr Ref<_Ty>& operator=(Ref<_Ty2>&& other) noexcept
        {
            Ref<_Ty>(std::move(other)).Swap(*this);
            return *this;
        }

        constexpr Ref<_Ty>& operator=(std::nullptr_t) noexcept
        {
            Ref<_Ty>(nullptr).Swap(*this);
            return *this;
        }

        constexpr _Ty* operator->() const noexcept { return Raw(); }
        constexpr _Ty& operator*() const noexcept { return *Raw(); }

    private:
        void IncRef() noexcept
        {
            if (m_RefCount)
                (void)m_RefCount->fetch_add(1, std::memory_order_relaxed);
        }

        void DecRef() noexcept
        {
            // Decrement the reference count by 1 and check if the reference count was 1 before decrementing
            if (m_RefCount && m_RefCount->fetch_sub(1, std::memory_order_acq_rel) == 1)
            {
                std::atomic_thread_fence(std::memory_order_acquire);

                if (m_Ptr)
                    delete m_Ptr;

                delete m_RefCount;
            }
        }

        void CopyFrom(const Ref<_Ty>& other) noexcept
        {
            if (this != &other)
            {
                m_Ptr = other.m_Ptr;
                m_RefCount = other.m_RefCount;

                IncRef();
            }
        }

        constexpr void ConstructFromWeakRef(const WeakRef<_Ty>* weakRef) noexcept { *this = *(Ref<_Ty>*)(weakRef); }

    private:
        template<typename _Ty2>
        friend class Ref;

        template<typename _Ty2>
        friend class WeakRef;

    private:
        _Ty* m_Ptr = nullptr;
        std::atomic_uint* m_RefCount = nullptr;
    };

    template<typename _Ty, typename... _Args, std::enable_if_t<std::negation_v<std::is_array<_Ty>>, int> = 0>
    constexpr inline static Ref<_Ty> CreateRef(_Args&&... args) noexcept
    {
        return Ref<_Ty>(new _Ty(std::forward<_Args>(args)...));
    }

    template<typename _WantedType, typename _RefType>
    constexpr inline static _WantedType* GetRefBaseTypePtr(const Ref<_RefType>& ref) noexcept
    {
        return static_cast<_WantedType*>(ref.Raw());
    }

    template<typename _Ty1, typename _Ty2>
    constexpr inline static bool operator==(const Ref<_Ty1>& left, const Ref<_Ty2>& right) noexcept
    {
        return left.Raw() == right.Raw();
    }

    template<typename _Ty1, typename _Ty2>
    constexpr inline static bool operator==(const Ref<_Ty1>& left, _Ty2* right) noexcept
    {
        return left.Raw() == right;
    }

    template<typename _Ty1, typename _Ty2>
    constexpr inline static bool operator==(_Ty1* left, const Ref<_Ty2> right) noexcept
    {
        return left == right.Raw();
    }

    template<typename _Ty>
    constexpr inline static bool operator==(const Ref<_Ty>& left, std::nullptr_t) noexcept
    {
        return left.Raw() == nullptr;
    }

    template<typename _Ty>
    constexpr inline static bool operator==(std::nullptr_t, const Ref<_Ty>& right) noexcept
    {
        return nullptr == right.Raw();
    }

    template<typename _Ty1, typename _Ty2>
    constexpr inline static bool operator!=(const Ref<_Ty1>& left, const Ref<_Ty2>& right) noexcept
    {
        return left.Raw() != right.Raw();
    }

    template<typename _Ty1, typename _Ty2>
    constexpr inline static bool operator!=(const Ref<_Ty1>& left, _Ty2* right) noexcept
    {
        return left.Raw() != right;
    }

    template<typename _Ty1, typename _Ty2>
    constexpr inline static bool operator!=(_Ty1* left, const Ref<_Ty2>& right) noexcept
    {
        return left != right.Raw();
    }

    template<typename _Ty>
    constexpr inline static bool operator!=(const Ref<_Ty>& left, std::nullptr_t) noexcept
    {
        return left.Raw() != nullptr;
    }

    template<typename _Ty>
    constexpr inline static bool operator!=(std::nullptr_t, const Ref<_Ty>& right) noexcept
    {
        return nullptr != right.Raw();
    }

    template<typename _Ty1, typename _Ty2>
    constexpr inline static bool operator<(const Ref<_Ty1>& left, const Ref<_Ty2>& right) noexcept
    {
        return left.Raw() < right.Raw();
    }

    template<typename _Ty1, typename _Ty2>
    constexpr inline static bool operator<(const Ref<_Ty1>& left, _Ty2* right) noexcept
    {
        return left.Raw() < right;
    }

    template<typename _Ty1, typename _Ty2>
    constexpr inline static bool operator<(_Ty1* left, const Ref<_Ty2>& right) noexcept
    {
        return left < right.Raw();
    }

    template<typename _Ty>
    constexpr inline static bool operator<(const Ref<_Ty>& left, std::nullptr_t) noexcept
    {
        return left.Raw() < static_cast<_Ty*>(nullptr);
    }

    template<typename _Ty>
    constexpr inline static bool operator<(std::nullptr_t, const Ref<_Ty>& right) noexcept
    {
        return static_cast<_Ty*>(nullptr) < right.Raw();
    }

    template<typename _Ty1, typename _Ty2>
    constexpr inline static bool operator<=(const Ref<_Ty1>& left, const Ref<_Ty2>& right) noexcept
    {
        return left.Raw() <= right.Raw();
    }

    template<typename _Ty1, typename _Ty2>
    constexpr inline static bool operator<=(const Ref<_Ty1>& left, _Ty2* right) noexcept
    {
        return left.Raw() <= right;
    }

    template<typename _Ty1, typename _Ty2>
    constexpr inline static bool operator<=(_Ty1* left, const Ref<_Ty2>& right) noexcept
    {
        return left <= right.Raw();
    }

    template<typename _Ty>
    constexpr inline static bool operator<=(const Ref<_Ty>& left, std::nullptr_t) noexcept
    {
        return left.Raw() <= static_cast<_Ty*>(nullptr);
    }

    template<typename _Ty>
    constexpr inline static bool operator<=(std::nullptr_t, const Ref<_Ty>& right) noexcept
    {
        return static_cast<_Ty*>(nullptr) <= right.Raw();
    }

    template<typename _Ty1, typename _Ty2>
    constexpr inline static bool operator>(const Ref<_Ty1>& left, const Ref<_Ty2>& right) noexcept
    {
        return left.Raw() > right.Raw();
    }

    template<typename _Ty1, typename _Ty2>
    constexpr inline static bool operator>(const Ref<_Ty1>& left, _Ty2* right) noexcept
    {
        return left.Raw() > right;
    }

    template<typename _Ty1, typename _Ty2>
    constexpr inline static bool operator>(_Ty1* left, const Ref<_Ty2>& right) noexcept
    {
        return left > right.Raw();
    }

    template<typename _Ty>
    constexpr inline static bool operator>(const Ref<_Ty>& left, std::nullptr_t) noexcept
    {
        return left.Raw() > static_cast<_Ty*>(nullptr);
    }

    template<typename _Ty>
    constexpr inline static bool operator>(std::nullptr_t, const Ref<_Ty>& right) noexcept
    {
        return static_cast<_Ty*>(nullptr) > right.Raw();
    }

    template<typename _Ty1, typename _Ty2>
    constexpr inline static bool operator>=(const Ref<_Ty1>& left, const Ref<_Ty2>& right) noexcept
    {
        return left.Raw() >= right.Raw();
    }

    template<typename _Ty1, typename _Ty2>
    constexpr inline static bool operator>=(const Ref<_Ty1>& left, _Ty2* right) noexcept
    {
        return left.Raw() >= right;
    }

    template<typename _Ty1, typename _Ty2>
    constexpr inline static bool operator>=(_Ty1* left, const Ref<_Ty2>& right) noexcept
    {
        return left >= right.Raw();
    }

    template<typename _Ty>
    constexpr inline static bool operator>=(const Ref<_Ty>& left, std::nullptr_t) noexcept
    {
        return left.Raw() >= static_cast<_Ty*>(nullptr);
    }

    template<typename _Ty>
    constexpr inline static bool operator>=(std::nullptr_t, const Ref<_Ty>& right) noexcept
    {
        return static_cast<_Ty*>(nullptr) >= right.Raw();
    }

    template<typename _Elem, typename _Traits, typename _Ty>
    constexpr inline static std::basic_ostream<_Elem, _Traits>& operator<<(std::basic_ostream<_Elem, _Traits>& ostream, const Ref<_Ty>& ptr)
    {
        return ostream << ptr.Raw();
    }

    template<typename _Ty>
    class WeakRef
    {
    public:
        constexpr WeakRef(const Ref<_Ty>& ref) noexcept : m_Ptr(ref.m_Ptr), m_RefCount(ref.m_RefCount) { };

        template<typename _Ty2, std::enable_if_t<std::is_base_of_v<_Ty, _Ty2>, int> = 0>
        constexpr WeakRef(const Ref<_Ty>& ref) noexcept : m_Ptr(ref.m_Ptr), m_RefCount(ref.m_RefCount) { };

        template<typename _Ty2, std::enable_if_t<std::is_base_of_v<_Ty, _Ty2>, int> = 0>
        constexpr WeakRef(const Ref<_Ty2>& ref) noexcept : m_Ptr(ref.m_Ptr), m_RefCount(ref.m_RefCount) { };

        template<typename _Ty2, std::enable_if_t<std::is_base_of_v<_Ty, _Ty2>, int> = 0>
        constexpr WeakRef(const WeakRef<_Ty2>& other) noexcept { CopyFrom(other); }

        constexpr WeakRef(const WeakRef<_Ty>& other) noexcept { CopyFrom(other); }

        template<typename _Ty2, std::enable_if_t<std::is_base_of_v<_Ty, _Ty2>, int> = 0>
        constexpr WeakRef(WeakRef<_Ty2>&& other) noexcept : m_Ptr(other.m_Ptr), m_RefCount(other.m_RefCount)
        {
            other.m_Ptr = nullptr;
            other.m_RefCount = nullptr;
        }

        constexpr WeakRef(WeakRef<_Ty>&& other) noexcept : m_Ptr(other.m_Ptr), m_RefCount(other.m_RefCount)
        {
            other.m_Ptr = nullptr;
            other.m_RefCount = nullptr;
        }

        constexpr WeakRef(std::nullptr_t) noexcept : m_Ptr(nullptr), m_RefCount(nullptr) { };
        WeakRef() = default;
        constexpr ~WeakRef() noexcept { Reset(); }

        constexpr void Swap(WeakRef<_Ty>& other) noexcept
        {
            if (this != &other)
            {
                std::swap(m_Ptr, other.m_Ptr);
                std::swap(m_RefCount, other.m_RefCount);
            }
        }

        constexpr void Reset() noexcept
        {
            m_Ptr = nullptr;
            m_RefCount = nullptr;
        }

        constexpr _Ty* Raw() const noexcept { return m_Ptr; }
        uint32_t RefCount() const noexcept { return m_RefCount ? m_RefCount->load() : 0; }

        bool Unique() const noexcept { return RefCount() == 1; }
        bool Expired() const noexcept { return RefCount() == 0; }

        Ref<_Ty> Lock() const noexcept
        {
            if (Expired())
                return nullptr;

            Ref<_Ty> res;
            res.ConstructFromWeakRef(this);

            return res;
        }

        constexpr bool Valid() const noexcept { return m_Ptr != nullptr; }
        constexpr explicit operator bool() const noexcept { return Valid(); }

        constexpr operator WeakRef<const _Ty>() const noexcept { return WeakRef<const _Ty>{ m_Ptr, m_RefCount }; }

        WeakRef<_Ty>& operator=(const WeakRef<_Ty>& other) noexcept
        {
            WeakRef<_Ty>(other).Swap(*this);

            return *this;
        }

        template<typename _Ty2, std::enable_if_t<std::is_base_of_v<_Ty, _Ty2>, int> = 0>
        constexpr WeakRef<_Ty>& operator=(const WeakRef<_Ty2>& other) noexcept
        {
            WeakRef<_Ty>(other).Swap(*this);

            return *this;
        }

        WeakRef<_Ty>& operator=(WeakRef<_Ty>&& other) noexcept
        {
            WeakRef<_Ty>(std::move(other)).Swap(*this);

            return *this;
        }

        template<typename _Ty2, std::enable_if_t<std::is_base_of_v<_Ty, _Ty2>, int> = 0>
        constexpr WeakRef<_Ty>& operator=(WeakRef<_Ty2>&& other) noexcept
        {
            WeakRef<_Ty>(std::move(other)).Swap(*this);

            return *this;
        }

        WeakRef<_Ty>& operator=(const Ref<_Ty>& ref) noexcept
        {
            WeakRef<_Ty>(ref).Swap(*this);

            return *this;
        }

        template<typename _Ty2, std::enable_if_t<std::is_base_of_v<_Ty, _Ty2>, int> = 0>
        constexpr WeakRef<_Ty>& operator=(const Ref<_Ty2>& ref) noexcept
        {
            WeakRef<_Ty>(ref).Swap(*this);

            return *this;
        }

        constexpr WeakRef<_Ty>& operator=(std::nullptr_t) noexcept
        {
            Reset();

            return *this;
        }

    private:
        constexpr void CopyFrom(const WeakRef<_Ty>& other) noexcept
        {
            if (this != &other)
            {
                m_Ptr = other.m_Ptr;
                m_RefCount = other.m_RefCount;
            }
        }

    private:
        template<typename _Ty2>
        friend class WeakRef;

    private:
        _Ty* m_Ptr;
        std::atomic_uint* m_RefCount;
    };

    template<typename _Ty1, typename _Ty2>
    constexpr inline static bool operator==(const WeakRef<_Ty1>& left, const WeakRef<_Ty2>& right) noexcept
    {
        return left.Raw() == right.Raw();
    }

    template<typename _Ty1, typename _Ty2>
    constexpr inline static bool operator==(const WeakRef<_Ty1>& left, _Ty2* right) noexcept
    {
        return left.Raw() == right;
    }

    template<typename _Ty1, typename _Ty2>
    constexpr inline static bool operator==(_Ty1* left, const WeakRef<_Ty2> right) noexcept
    {
        return left == right.Raw();
    }

    template<typename _Ty>
    constexpr inline static bool operator==(const WeakRef<_Ty>& left, std::nullptr_t) noexcept
    {
        return left.Raw() == nullptr;
    }

    template<typename _Ty>
    constexpr inline static bool operator==(std::nullptr_t, const WeakRef<_Ty>& right) noexcept
    {
        return nullptr == right.Raw();
    }

    template<typename _Ty1, typename _Ty2>
    constexpr inline static bool operator!=(const WeakRef<_Ty1>& left, const WeakRef<_Ty2>& right) noexcept
    {
        return left.Raw() != right.Raw();
    }

    template<typename _Ty1, typename _Ty2>
    constexpr inline static bool operator!=(const WeakRef<_Ty1>& left, _Ty2* right) noexcept
    {
        return left.Raw() != right;
    }

    template<typename _Ty1, typename _Ty2>
    constexpr inline static bool operator!=(_Ty1* left, const WeakRef<_Ty2>& right) noexcept
    {
        return left != right.Raw();
    }

    template<typename _Ty>
    constexpr inline static bool operator!=(const WeakRef<_Ty>& left, std::nullptr_t) noexcept
    {
        return left.Raw() != nullptr;
    }

    template<typename _Ty>
    constexpr inline static bool operator!=(std::nullptr_t, const WeakRef<_Ty>& right) noexcept
    {
        return nullptr != right.Raw();
    }

    template<typename _Ty1, typename _Ty2>
    constexpr inline static bool operator<(const Ref<_Ty1>& left, const WeakRef<_Ty2>& right) noexcept
    {
        return left.Raw() < right.Raw();
    }

    template<typename _Ty1, typename _Ty2>
    constexpr inline static bool operator<(const WeakRef<_Ty1>& left, _Ty2* right) noexcept
    {
        return left.Raw() < right;
    }

    template<typename _Ty1, typename _Ty2>
    constexpr inline static bool operator<(_Ty1* left, const WeakRef<_Ty2>& right) noexcept
    {
        return left < right.Raw();
    }

    template<typename _Ty>
    constexpr inline static bool operator<(const WeakRef<_Ty>& left, std::nullptr_t) noexcept
    {
        return left.Raw() < static_cast<_Ty*>(nullptr);
    }

    template<typename _Ty>
    constexpr inline static bool operator<(std::nullptr_t, const WeakRef<_Ty>& right) noexcept
    {
        return static_cast<_Ty*>(nullptr) < right.Raw();
    }

    template<typename _Ty1, typename _Ty2>
    constexpr inline static bool operator<=(const WeakRef<_Ty1>& left, const WeakRef<_Ty2>& right) noexcept
    {
        return left.Raw() <= right.Raw();
    }

    template<typename _Ty1, typename _Ty2>
    constexpr inline static bool operator<=(const WeakRef<_Ty1>& left, _Ty2* right) noexcept
    {
        return left.Raw() <= right;
    }

    template<typename _Ty1, typename _Ty2>
    constexpr inline static bool operator<=(_Ty1* left, const WeakRef<_Ty2>& right) noexcept
    {
        return left <= right.Raw();
    }

    template<typename _Ty>
    constexpr inline static bool operator<=(const WeakRef<_Ty>& left, std::nullptr_t) noexcept
    {
        return left.Raw() <= static_cast<_Ty*>(nullptr);
    }

    template<typename _Ty>
    constexpr inline static bool operator<=(std::nullptr_t, const WeakRef<_Ty>& right) noexcept
    {
        return static_cast<_Ty*>(nullptr) <= right.Raw();
    }

    template<typename _Ty1, typename _Ty2>
    constexpr inline static bool operator>(const WeakRef<_Ty1>& left, const WeakRef<_Ty2>& right) noexcept
    {
        return left.Raw() > right.Raw();
    }

    template<typename _Ty1, typename _Ty2>
    constexpr inline static bool operator>(const WeakRef<_Ty1>& left, _Ty2* right) noexcept
    {
        return left.Raw() > right;
    }

    template<typename _Ty1, typename _Ty2>
    constexpr inline static bool operator>(_Ty1* left, const WeakRef<_Ty2>& right) noexcept
    {
        return left > right.Raw();
    }

    template<typename _Ty>
    constexpr inline static bool operator>(const WeakRef<_Ty>& left, std::nullptr_t) noexcept
    {
        return left.Raw() > static_cast<_Ty*>(nullptr);
    }

    template<typename _Ty>
    constexpr inline static bool operator>(std::nullptr_t, const WeakRef<_Ty>& right) noexcept
    {
        return static_cast<_Ty*>(nullptr) > right.Raw();
    }

    template<typename _Ty1, typename _Ty2>
    constexpr inline static bool operator>=(const WeakRef<_Ty1>& left, const WeakRef<_Ty2>& right) noexcept
    {
        return left.Raw() >= right.Raw();
    }

    template<typename _Ty1, typename _Ty2>
    constexpr inline static bool operator>=(const WeakRef<_Ty1>& left, _Ty2* right) noexcept
    {
        return left.Raw() >= right;
    }

    template<typename _Ty1, typename _Ty2>
    constexpr inline static bool operator>=(_Ty1* left, const WeakRef<_Ty2>& right) noexcept
    {
        return left >= right.Raw();
    }

    template<typename _Ty>
    constexpr inline static bool operator>=(const WeakRef<_Ty>& left, std::nullptr_t) noexcept
    {
        return left.Raw() >= static_cast<_Ty*>(nullptr);
    }

    template<typename _Ty>
    constexpr inline static bool operator>=(std::nullptr_t, const WeakRef<_Ty>& right) noexcept
    {
        return static_cast<_Ty*>(nullptr) >= right.Raw();
    }

    template<typename _Elem, typename _Traits, typename _Ty>
    constexpr inline static std::basic_ostream<_Elem, _Traits>& operator<<(std::basic_ostream<_Elem, _Traits>& ostream, const WeakRef<_Ty>& ptr)
    {
        return ostream << ptr.Raw();
    }

    template<typename _Ty>
    using UniquePtr = std::unique_ptr<_Ty>;

    template<typename _Ty, typename... _Args>
    constexpr inline static UniquePtr<_Ty> CreateUniquePtr(_Args&&... args) noexcept
    {
        return std::make_unique<_Ty>(std::forward<_Args>(args)...);
    }

    template<typename _Ty>
    using SharedPtr = std::shared_ptr<_Ty>;

    template<typename _Ty, typename... _Args>
    constexpr inline static SharedPtr<_Ty> CreateSharedPtr(_Args&&... args) noexcept
    {
        return std::make_shared<_Ty>(std::forward<_Args>(args)...);
    }

    template<typename _Ty>
    using WeakPtr = std::weak_ptr<_Ty>;
}

namespace std
{
    template<class _Kty>
    struct hash;

    template<typename _Ty>
    struct hash<Intricate::Scope<_Ty>>
    {
        size_t operator()(const Intricate::Scope<_Ty>& ptr) const noexcept { return hash<_Ty*>{}(ptr.Raw()); }
    };

    template<typename _Ty>
    struct hash<Intricate::Ref<_Ty>>
    {
        size_t operator()(const Intricate::Ref<_Ty>& ptr) const noexcept { return hash<_Ty*>{}(ptr.Raw()); }
    };

    template<typename _Ty>
    struct hash<Intricate::WeakRef<_Ty>>
    {
        size_t operator()(const Intricate::WeakRef<_Ty>& ptr) const noexcept { return hash<_Ty*>{}(ptr.Raw()); }
    };
}
