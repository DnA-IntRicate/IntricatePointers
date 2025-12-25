/**************************************************************************
 * IntricatePointers: https://github.com/DnA-IntRicate/IntricatePointers
 * A single-header containing smart pointer implementations in C++20
 * ------------------------------------------------------------------------
 * Copyright 2025 Adam Foflonker
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http ://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 **************************************************************************/

#pragma once

#ifdef INTRICATE_BUILD_MODULE
#   define INTRICATE_EXPORT export

import <ostream>;
import <type_traits>;
import <memory>;
import <atomic>;
import <utility>;
import <cstdint>;
import <functional>;
#else
#   define INTRICATE_EXPORT

#   include <ostream>
#   include <type_traits>
#   include <memory>
#   include <atomic>
#   include <utility>
#   include <cstdint>
#   include <functional>
#endif

#ifndef INTRICATE_OMIT_NAMESPACE
#   define INTRICATE_NAMESPACE_BEGIN INTRICATE_EXPORT namespace Intricate {
#   define INTRICATE_NAMESPACE_END }
#   define _INTRICATE ::Intricate::
#else
#   define INTRICATE_NAMESPACE_BEGIN
#   define INTRICATE_NAMESPACE_END
#   define _INTRICATE
#endif // !INTRICATE_OMIT_NAMESPACE

INTRICATE_NAMESPACE_BEGIN

INTRICATE_EXPORT template<typename _Ty>
class Scope
{
public:
    constexpr explicit Scope(_Ty* ptr) noexcept : m_Ptr(ptr), m_Deleter(&_DefaultDelete<_Ty>) {};

    template<typename _Ty2, std::enable_if_t<std::is_base_of_v<_Ty, _Ty2>, int> = 0>
    constexpr explicit Scope(_Ty2* ptr) noexcept : m_Ptr(ptr), m_Deleter(&_DefaultDelete<_Ty2>) {};

    template<typename _Ty2, std::enable_if_t<std::is_base_of_v<_Ty, _Ty2>, int> = 0>
    constexpr Scope(Scope<_Ty2>&& scope) noexcept : m_Ptr(scope.Release()), m_Deleter(std::exchange(scope.m_Deleter, nullptr)) {};

    constexpr Scope(Scope<_Ty>&& scope) noexcept : m_Ptr(scope.Release()), m_Deleter(std::exchange(scope.m_Deleter, nullptr)) {};

    Scope(Scope<_Ty>&) = delete;
    Scope(const Scope<_Ty>&) = delete;
    constexpr Scope(std::nullptr_t) noexcept : m_Ptr(nullptr), m_Deleter(nullptr) {};
    constexpr Scope() noexcept = default;

    constexpr ~Scope() noexcept
    {
        if (m_Ptr)
        {
            if (m_Deleter)
                m_Deleter((void*)m_Ptr);

            m_Ptr = nullptr;
        }
    }

    constexpr void Swap(Scope<_Ty>& other) noexcept
    {
        if (this != &other)
        {
            std::swap(m_Ptr, other.m_Ptr);
            std::swap(m_Deleter, other.m_Deleter);
        }
    }

    template<typename _Ty2, std::enable_if_t<std::is_base_of_v<_Ty, _Ty2>, int> = 0>
    constexpr void Reset(_Ty2* newPtr) noexcept
    {
        Scope<_Ty2>(newPtr).Swap(*this);
    }

    constexpr void Reset(_Ty* newPtr) noexcept
    {
        Scope<_Ty>(newPtr).Swap(*this);
    }

    constexpr void Reset() noexcept
    {
        Scope<_Ty>(nullptr).Swap(*this);
    }

    constexpr _Ty* Release() noexcept
    {
        return std::exchange(m_Ptr, nullptr);
    }

    constexpr _Ty* Raw() const noexcept
    {
        return m_Ptr;
    }

    constexpr bool Valid() const noexcept
    {
        return Raw() != nullptr;
    }

    constexpr explicit operator bool() const noexcept { return Valid(); }

    Scope<_Ty>& operator=(Scope<_Ty>&) = delete;
    Scope<_Ty>& operator=(const Scope<_Ty>&) = delete;

    constexpr Scope<_Ty>& operator=(Scope<_Ty>&& other) noexcept
    {
        Scope<_Ty>(std::move(other)).Swap(*this);
        return *this;
    }

    template<typename _Ty2, std::enable_if_t<std::is_base_of_v<_Ty, _Ty2>, int> = 0>
    constexpr Scope<_Ty>& operator=(Scope<_Ty2>&& other) noexcept
    {
        Scope<_Ty>(std::move(other)).Swap(*this);
        return *this;
    }

    constexpr Scope<_Ty>& operator=(std::nullptr_t) noexcept
    {
        Scope<_Ty>(nullptr).Swap(*this);
        return *this;
    }

    template<typename _Ty2 = _Ty, std::enable_if_t<!std::is_void_v<_Ty2>, int> = 0>
    constexpr _Ty2* operator->() const noexcept { return Raw(); }

    template<typename _Ty2 = _Ty, std::enable_if_t<!std::is_void_v<_Ty2>, int> = 0>
    constexpr _Ty2& operator*() const noexcept { return *Raw(); }

private:
    template<typename _Ty2>
    static void _DefaultDelete(void* ptr) noexcept
    {
        delete static_cast<_Ty2*>(ptr);
    }

private:
    template<typename _Ty2>
    friend class Scope;

private:
    _Ty* m_Ptr = nullptr;
    void (*m_Deleter)(void*) = nullptr; // Type-erased deleter
};

INTRICATE_EXPORT template<typename _Ty, typename... _Args, std::enable_if_t<!std::is_array_v<_Ty>, int> = 0>
constexpr Scope<_Ty> CreateScope(_Args&&... args) noexcept
{
    return Scope<_Ty>(new _Ty(std::forward<_Args>(args)...));
}

INTRICATE_EXPORT template<typename _WantedType, typename _ScopeType>
_WantedType* GetScopeBaseTypePtr(const Scope<_ScopeType>& scope) noexcept
{
    return static_cast<_WantedType*>(scope.Raw());
}

class _AtomicRefCount
{
public:
    constexpr _AtomicRefCount() noexcept = default;
    constexpr ~_AtomicRefCount() noexcept = default;

    _AtomicRefCount(const _AtomicRefCount&) = delete;
    _AtomicRefCount& operator=(const _AtomicRefCount&) = delete;

    uint32_t GetStrongs() const noexcept
    {
        return m_Strongs.load(std::memory_order_acquire);
    }

    uint32_t GetWeaks() const noexcept
    {
        return m_Weaks.load(std::memory_order_acquire);
    }

    uint32_t IncRef() noexcept
    {
        return m_Strongs.fetch_add(1, std::memory_order_relaxed) + 1;
    }

    uint32_t DecRef() noexcept
    {
        return m_Strongs.fetch_sub(1, std::memory_order_acq_rel) - 1;
    }

    uint32_t IncWeakRef() noexcept
    {
        return m_Weaks.fetch_add(1, std::memory_order_relaxed) + 1;
    }

    uint32_t DecWeakRef() noexcept
    {
        return m_Weaks.fetch_sub(1, std::memory_order_acq_rel) - 1;
    }

private:
    std::atomic_uint m_Strongs = 1;
    std::atomic_uint m_Weaks = 0;
};

template<typename _Ty>
class Ref;

template<typename _Ty>
class WeakRef;

// Base class for Ref, UnsafeRef and WeakRef
// std::remove_extent<> will need to be used in future to support array types.
template<typename _Ty>
class _RefBase
{
protected:
    constexpr _RefBase(std::nullptr_t) noexcept : m_Ptr(nullptr), m_RefCount(nullptr), m_Deleter(nullptr) {};
    constexpr _RefBase() noexcept = default;
    constexpr ~_RefBase() noexcept = default;

public:
    _RefBase(const _RefBase<_Ty>&) = delete;
    _RefBase<_Ty>& operator=(const _RefBase<_Ty>&) = delete;

protected:
    constexpr _Ty* _Raw() const noexcept
    {
        return m_Ptr;
    }

    constexpr void _Swap(_RefBase<_Ty>& other) noexcept
    {
        std::swap(m_Ptr, other.m_Ptr);
        std::swap(m_RefCount, other.m_RefCount);
        std::swap(m_Deleter, other.m_Deleter);
    }

    uint32_t _RefCount() const noexcept
    {
        return m_RefCount ? m_RefCount->GetStrongs() : 0;
    }

    uint32_t _WeakRefCount() const noexcept
    {
        return m_RefCount ? m_RefCount->GetWeaks() : 0;
    }

    void _IncRef() noexcept
    {
        if (m_RefCount)
            (void)m_RefCount->IncRef();
    }

    void _DecRef() noexcept
    {
        if (m_RefCount && (m_RefCount->DecRef() == 0))
        {
            if (m_Ptr)
            {
                if (m_Deleter)
                    m_Deleter((void*)m_Ptr);

                m_Ptr = nullptr;
            }

            if (_WeakRefCount() == 0)
            {
                delete m_RefCount;
                m_RefCount = nullptr;
            }
        }
    }

    void _IncWeakRef() noexcept
    {
        if (m_RefCount)
            (void)m_RefCount->IncWeakRef();
    }

    void _DecWeakRef() noexcept
    {
        if (m_RefCount && (m_RefCount->DecWeakRef() == 0) && (_RefCount() == 0))
        {
            delete m_RefCount;
            m_RefCount = nullptr;
        }
    }

    template<typename _Ty2>
    constexpr void _ConstructFromRaw(_Ty2* ptr) noexcept
    {
        m_Ptr = static_cast<_Ty*>(ptr);
        m_RefCount = m_Ptr ? new _AtomicRefCount() : nullptr;
        m_Deleter = m_Ptr ? &_DefaultDelete<_Ty2> : nullptr;
    }

    template<typename _Ty2, typename _Ty3>
    constexpr void _AliasConstructFrom(_Ty2* ptr, const Ref<_Ty3>& ref) noexcept
    {
        m_Ptr = static_cast<_Ty*>(ptr);
        m_RefCount = ref.m_RefCount;
        m_Deleter = ref.m_Deleter;

        // Guard against incrementing if we alias-constructed from an empty ref
        if (m_RefCount)
            _IncRef();
    }

    template<typename _Ty2, typename _Ty3>
    constexpr void _MoveAliasConstructFrom(_Ty2* ptr, Ref<_Ty3>&& ref) noexcept
    {
        m_Ptr = static_cast<_Ty*>(ptr);
        m_RefCount = ref.m_RefCount;
        m_Deleter = ref.m_Deleter;

        ref.m_Ptr = nullptr;
        ref.m_RefCount = nullptr;
        ref.m_Deleter = nullptr;
    }

    template<typename _Ty2>
    constexpr void _MoveConstructFrom(_RefBase<_Ty2>&& ptr) noexcept
    {
        m_Ptr = static_cast<_Ty*>(ptr.m_Ptr);
        m_RefCount = ptr.m_RefCount;
        m_Deleter = ptr.m_Deleter;

        ptr.m_Ptr = nullptr;
        ptr.m_RefCount = nullptr;
        ptr.m_Deleter = nullptr;
    }

    template<typename _Ty2>
    constexpr void _CopyConstructFrom(const Ref<_Ty2>& ref) noexcept
    {
        m_Ptr = static_cast<_Ty*>(ref.m_Ptr);
        m_RefCount = ref.m_RefCount;
        m_Deleter = ref.m_Deleter;

        _IncRef();
    }

    template<typename _Ty2>
    constexpr void _WeaklyConstructFrom(const _RefBase<_Ty2>& ptr) noexcept
    {
        m_Ptr = static_cast<_Ty*>(ptr.m_Ptr);
        m_RefCount = ptr.m_RefCount;
        m_Deleter = ptr.m_Deleter;

        _IncWeakRef();
    }

    template<typename _Ty2>
    constexpr void _ConstructFromWeak(const WeakRef<_Ty2>& weak) noexcept
    {
        // We cannot create a strong reference to the resource if it was already deleted
        if (weak._RefCount() == 0)
        {
            m_Ptr = nullptr;
            m_RefCount = nullptr;
            m_Deleter = nullptr;
            return;
        }

        m_Ptr = static_cast<_Ty*>(weak.m_Ptr);
        m_RefCount = weak.m_RefCount;
        m_Deleter = weak.m_Deleter;

        _IncRef();
    }

    template<typename _Ty2>
    constexpr void _UnsafelyConstructFrom(const _RefBase<_Ty2>& ptr) noexcept
    {
        m_Ptr = static_cast<_Ty*>(ptr.m_Ptr);
        m_RefCount = ptr.m_RefCount;
        m_Deleter = ptr.m_Deleter;
    }

private:
    template<typename _Ty2>
    static void _DefaultDelete(void* ptr) noexcept
    {
        delete static_cast<_Ty2*>(ptr);
    }

private:
    _Ty* m_Ptr = nullptr;
    _AtomicRefCount* m_RefCount = nullptr;
    void (*m_Deleter)(void*) = nullptr; // Type-erased deleter

private:
    template<typename _Ty2>
    friend class _RefBase;

    friend class Ref<_Ty>;
    friend class WeakRef<_Ty>;
};

INTRICATE_EXPORT template<typename _Ty>
class Ref : public _RefBase<_Ty>
{
public:
    constexpr explicit Ref(_Ty* ptr) noexcept { this->_ConstructFromRaw(ptr); }

    template<typename _Ty2, std::enable_if_t<std::is_base_of_v<_Ty, _Ty2>, int> = 0>
    constexpr explicit Ref(_Ty2* ptr) noexcept { this->_ConstructFromRaw(ptr); }

    template<typename _Ty2, std::enable_if_t<std::is_base_of_v<_Ty, _Ty2>, int> = 0>
    constexpr Ref(const Ref<_Ty2>& ref) noexcept { this->_CopyConstructFrom(ref); }

    Ref(const Ref<_Ty>& ref) noexcept { this->_CopyConstructFrom(ref); }

    template<typename _Ty2, typename _Ty3>
    constexpr Ref(_Ty2* newPtr, const Ref<_Ty3>& owner) noexcept { this->_AliasConstructFrom(newPtr, owner); }

    template<typename _Ty2, typename _Ty3>
    constexpr Ref(_Ty2* newPtr, Ref<_Ty3>&& owner) noexcept { this->_MoveAliasConstructFrom(newPtr, std::move(owner)); }

    template<typename _Ty2, std::enable_if_t<std::is_base_of_v<_Ty, _Ty2>, int> = 0>
    constexpr Ref(Ref<_Ty2>&& ref) noexcept { this->_MoveConstructFrom(std::move(ref)); }

    constexpr Ref(Ref<_Ty>&& ref) noexcept { this->_MoveConstructFrom(std::move(ref)); }

    template<typename _Ty2, std::enable_if_t<std::is_base_of_v<_Ty, _Ty2>, int> = 0>
    constexpr explicit Ref(const WeakRef<_Ty2>& weak) noexcept { this->_ConstructFromWeak(weak); }

    constexpr explicit Ref(const WeakRef<_Ty>& weak) noexcept { this->_ConstructFromWeak(weak); }

    constexpr Ref(std::nullptr_t) noexcept : _RefBase<_Ty>(nullptr) {};
    constexpr Ref() noexcept = default;
    constexpr ~Ref() noexcept { this->_DecRef(); }

    constexpr void Swap(Ref<_Ty>& other) noexcept
    {
        if (this != &other)
            this->_Swap(other);
    }

    template<typename _Ty2, std::enable_if_t<std::is_base_of_v<_Ty, _Ty2>, int> = 0>
    constexpr void Reset(_Ty2* newPtr) noexcept
    {
        Ref<_Ty2>(newPtr).Swap(*this);
    }

    constexpr void Reset(_Ty* newPtr) noexcept
    {
        Ref<_Ty>(newPtr).Swap(*this);
    }

    constexpr void Reset() noexcept
    {
        Ref<_Ty>(nullptr).Swap(*this);
    }

    constexpr _Ty* Release() noexcept
    {
        _Ty* res = std::exchange(this->m_Ptr, nullptr);
        Ref<_Ty>(nullptr).Swap(*this);

        return res;
    }

    uint32_t RefCount() const noexcept
    {
        return this->_RefCount();
    }

    bool Unique() const noexcept
    {
        return RefCount() == 1;
    }

    constexpr _Ty* Raw() const noexcept
    {
        return this->_Raw();
    }

    constexpr bool Valid() const noexcept
    {
        return Raw() != nullptr;
    }

    constexpr explicit operator bool() const noexcept { return Valid(); }
    constexpr operator Ref<const _Ty>() const noexcept { return Ref<const _Ty>{ this->m_Ptr, this->m_RefCount, this->m_Deleter }; }

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

    template<typename _Ty2 = _Ty, std::enable_if_t<!std::is_void_v<_Ty2>, int> = 0>
    constexpr _Ty2* operator->() const noexcept { return this->Raw(); }

    template<typename _Ty2 = _Ty, std::enable_if_t<!std::is_void_v<_Ty2>, int> = 0>
    constexpr _Ty2& operator*() const noexcept { return *Raw(); }

private:
    template<typename _Ty2>
    friend class Ref;
};

INTRICATE_EXPORT template<typename _Ty, typename... _Args, std::enable_if_t<!std::is_array_v<_Ty>, int> = 0>
constexpr Ref<_Ty> CreateRef(_Args&&... args) noexcept
{
    return Ref<_Ty>(new _Ty(std::forward<_Args>(args)...));
}

INTRICATE_EXPORT template<typename _WantedType, typename _RefType>
_WantedType* GetRefBaseTypePtr(const Ref<_RefType>& ref) noexcept
{
    return static_cast<_WantedType*>(ref.Raw());
}

// A non-owning view that grants temporary direct access to reference count
// manipulation for an existing Ref.
//
// This type does not participate in ownership or lifetime management. It may be
// used to increment or decrement the underlying object's reference count when
// higher-level ownership semantics are not suitable.
//
// Important notes:
//  * The referenced object must remain valid for the entire lifetime of this
//    instance. This class makes no guarantees about lifetime safety.
//  * Misuse can cause leaks, premature destruction, or inconsistencies in
//    ownership models. It should only be used when a clear manual strategy for
//    reference handling exists.
//  * Constructing this type is an explicit opt-in to bypassing automatic
//    lifetime management. Only cast the magic unsafe spell if you know what you're doing!
//
// Use with extreme caution, for this type is dangerous...
INTRICATE_EXPORT template<typename _Ty>
class UnsafeRef : public _RefBase<_Ty>
{
public:
    constexpr UnsafeRef(const Ref<_Ty>& ref) noexcept { this->_UnsafelyConstructFrom(ref); }

    template<typename _Ty2, std::enable_if_t<std::is_base_of_v<_Ty, _Ty2>, int> = 0>
    constexpr UnsafeRef(const Ref<_Ty2>& ref) noexcept { this->_UnsafelyConstructFrom(ref); }

    constexpr UnsafeRef(std::nullptr_t) noexcept : _RefBase<_Ty>(nullptr) {};
    constexpr UnsafeRef() noexcept = default;
    constexpr ~UnsafeRef() noexcept = default;

    constexpr void Swap(UnsafeRef<_Ty>& other) noexcept
    {
        if (this != &other)
            this->_Swap(other);
    }

    void IncRef() noexcept { this->_IncRef(); }
    void DecRef() noexcept { this->_DecRef(); }

    void IncWeakRef() noexcept { this->_IncWeakRef(); }
    void DecWeakRef() noexcept { this->_DecWeakRef(); }

    uint32_t RefCount() const noexcept
    {
        return this->_RefCount();
    }

    uint32_t WeakRefCount() const noexcept
    {
        return this->_WeakRefCount();
    }

    constexpr bool Valid() const noexcept
    {
        return this->_Raw() != nullptr;
    }

    constexpr explicit operator bool() const noexcept { return Valid(); }

    constexpr UnsafeRef<_Ty>& operator=(std::nullptr_t) noexcept
    {
        UnsafeRef<_Ty>(nullptr).Swap(*this);
        return *this;
    }

    // NOTE: To prevent accidental usages, we intentionally deleted the Ref assignment operator overloads.
    constexpr UnsafeRef<_Ty>& operator=(const Ref<_Ty>&) noexcept = delete;
    constexpr UnsafeRef<_Ty>& operator=(Ref<_Ty>&&) noexcept = delete;

private:
    template<typename _Ty2>
    friend class UnsafeRef;
};

// Unsafe cast and copy
INTRICATE_EXPORT template<typename _Ty>
UnsafeRef<_Ty> UnsafeRefCast(const Ref<_Ty>& ref) noexcept
{
    return UnsafeRef<_Ty>(ref);
}

// Static cast and copy
INTRICATE_EXPORT template<typename _WantedType, typename _RefType>
Ref<_WantedType> StaticRefCast(const Ref<_RefType>& ref) noexcept
{
    const auto ptr = static_cast<_WantedType*>(ref.Raw());
    return Ref<_WantedType>(ptr, ref);
}

// Static cast and move
INTRICATE_EXPORT template<typename _WantedType, typename _RefType>
Ref<_WantedType> StaticRefCast(Ref<_RefType>&& ref) noexcept
{
    const auto ptr = static_cast<_WantedType*>(ref.Raw());
    return Ref<_WantedType>(ptr, std::move(ref));
}

// Dynamic cast and copy
INTRICATE_EXPORT template<typename _WantedType, typename _RefType>
Ref<_WantedType> DynamicRefCast(const Ref<_RefType>& ref) noexcept
{
    const auto ptr = dynamic_cast<_WantedType*>(ref.Raw());
    return Ref<_WantedType>(ptr, ref);
}

// Dynamic cast and move
INTRICATE_EXPORT template<typename _WantedType, typename _RefType>
Ref<_WantedType> DynamicRefCast(Ref<_RefType>&& ref) noexcept
{
    const auto ptr = dynamic_cast<_WantedType*>(ref.Raw());
    return Ref<_WantedType>(ptr, std::move(ref));
}

// Reinterpret cast and copy
INTRICATE_EXPORT template<typename _WantedType, typename _RefType>
Ref<_WantedType> ReinterpretRefCast(const Ref<_RefType>& ref) noexcept
{
    const auto ptr = reinterpret_cast<_WantedType*>(ref.Raw());
    return Ref<_WantedType>(ptr, ref);
}

// Reinterpret cast and move
INTRICATE_EXPORT template<typename _WantedType, typename _RefType>
Ref<_WantedType> ReinterpretRefCast(Ref<_RefType>&& ref) noexcept
{
    const auto ptr = reinterpret_cast<_WantedType*>(ref.Raw());
    return Ref<_WantedType>(ptr, std::move(ref));
}

// Const cast and copy
INTRICATE_EXPORT template<typename _WantedType, typename _RefType>
Ref<_WantedType> ConstRefCast(const Ref<_RefType>& ref) noexcept
{
    const auto ptr = const_cast<_WantedType*>(ref.Raw());
    return Ref<_WantedType>(ptr, ref);
}

// Const cast and move
INTRICATE_EXPORT template<typename _WantedType, typename _RefType>
Ref<_WantedType> ConstRefCast(Ref<_RefType>&& ref) noexcept
{
    const auto ptr = const_cast<_WantedType*>(ref.Raw());
    return Ref<_WantedType>(ptr, std::move(ref));
}

INTRICATE_EXPORT template<typename _Ty>
class WeakRef : public _RefBase<_Ty>
{
public:
    constexpr WeakRef(const Ref<_Ty>& ref) noexcept { this->_WeaklyConstructFrom(ref); }

    template<typename _Ty2, std::enable_if_t<std::is_base_of_v<_Ty, _Ty2>, int> = 0>
    constexpr WeakRef(const Ref<_Ty2>& ref) noexcept { this->_WeaklyConstructFrom(ref); }

    template<typename _Ty2, std::enable_if_t<std::is_base_of_v<_Ty, _Ty2>, int> = 0>
    constexpr WeakRef(const WeakRef<_Ty2>& weakRef) noexcept { this->_WeaklyConstructFrom(weakRef); }

    constexpr WeakRef(const WeakRef<_Ty>& weakRef) noexcept { this->_WeaklyConstructFrom(weakRef); }

    template<typename _Ty2, std::enable_if_t<std::is_base_of_v<_Ty, _Ty2>, int> = 0>
    constexpr WeakRef(WeakRef<_Ty2>&& weakRef) noexcept { this->_MoveConstructFrom(std::move(weakRef)); }

    constexpr WeakRef(WeakRef<_Ty>&& weakRef) noexcept { this->_MoveConstructFrom(std::move(weakRef)); }

    constexpr WeakRef(std::nullptr_t) noexcept : _RefBase<_Ty>(nullptr) {};
    constexpr WeakRef() noexcept = default;
    constexpr ~WeakRef() noexcept { this->_DecWeakRef(); }

    constexpr void Swap(WeakRef<_Ty>& other) noexcept
    {
        if (this != &other)
            this->_Swap(other);
    }

    constexpr void Reset() noexcept
    {
        WeakRef<_Ty>(nullptr).Swap(*this);
    }

    uint32_t RefCount() const noexcept
    {
        return this->_RefCount();
    }

    bool Unique() const noexcept
    {
        return RefCount() == 1;
    }

    bool Expired() const noexcept
    {
        return RefCount() == 0;
    }

    constexpr _Ty* Raw() const noexcept
    {
        return this->_Raw();
    }

    constexpr bool Valid() const noexcept
    {
        return Raw() != nullptr;
    }

    Ref<_Ty> Lock() const noexcept
    {
        if (Expired())
            return nullptr;

        Ref<_Ty> res;
        res._ConstructFromWeak(*this);

        return res;
    }

    constexpr explicit operator bool() const noexcept { return Valid(); }
    constexpr operator WeakRef<const _Ty>() const noexcept { return WeakRef<const _Ty>{ this->m_Ptr, this->m_RefCount, this->m_Deleter }; }

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
        WeakRef<_Ty>(nullptr).Swap(*this);
        return *this;
    }

private:
    template<typename _Ty2>
    friend class WeakRef;
};

INTRICATE_EXPORT template<typename _Ty>
using UniquePtr = std::unique_ptr<_Ty>;

INTRICATE_EXPORT template<typename _Ty, typename... _Args>
constexpr UniquePtr<_Ty> CreateUniquePtr(_Args&&... args) noexcept
{
    return std::make_unique<_Ty>(std::forward<_Args>(args)...);
}

INTRICATE_EXPORT template<typename _Ty>
using SharedPtr = std::shared_ptr<_Ty>;

INTRICATE_EXPORT template<typename _Ty, typename... _Args>
constexpr SharedPtr<_Ty> CreateSharedPtr(_Args&&... args) noexcept
{
    return std::make_shared<_Ty>(std::forward<_Args>(args)...);
}

INTRICATE_EXPORT template<typename _Ty>
using WeakPtr = std::weak_ptr<_Ty>;

INTRICATE_EXPORT template<typename _Elem, typename _Traits, typename _Ty>
std::basic_ostream<_Elem, _Traits>& operator<<(std::basic_ostream<_Elem, _Traits>& ostream, const Scope<_Ty>& ptr)
{
    return ostream << ptr.Raw();
}

INTRICATE_EXPORT template<typename _Elem, typename _Traits, typename _Ty>
std::basic_ostream<_Elem, _Traits>& operator<<(std::basic_ostream<_Elem, _Traits>& ostream, const Ref<_Ty>& ptr)
{
    return ostream << ptr.Raw();
}

INTRICATE_EXPORT template<typename _Elem, typename _Traits, typename _Ty>
std::basic_ostream<_Elem, _Traits>& operator<<(std::basic_ostream<_Elem, _Traits>& ostream, const WeakRef<_Ty>& ptr)
{
    return ostream << ptr.Raw();
}

INTRICATE_EXPORT template<typename _Ty1, typename _Ty2>
bool operator==(const Ref<_Ty1>& left, const Ref<_Ty2>& right) noexcept
{
    return left.Raw() == right.Raw();
}

INTRICATE_EXPORT template<typename _Ty1, typename _Ty2>
bool operator==(const Ref<_Ty1>& left, _Ty2* right) noexcept
{
    return left.Raw() == right;
}

INTRICATE_EXPORT template<typename _Ty1, typename _Ty2>
bool operator==(_Ty1* left, const Ref<_Ty2> right) noexcept
{
    return left == right.Raw();
}

INTRICATE_EXPORT template<typename _Ty>
bool operator==(const Ref<_Ty>& left, std::nullptr_t) noexcept
{
    return left.Raw() == nullptr;
}

INTRICATE_EXPORT template<typename _Ty>
bool operator==(std::nullptr_t, const Ref<_Ty>& right) noexcept
{
    return nullptr == right.Raw();
}

INTRICATE_EXPORT template<typename _Ty1, typename _Ty2>
bool operator!=(const Ref<_Ty1>& left, const Ref<_Ty2>& right) noexcept
{
    return left.Raw() != right.Raw();
}

INTRICATE_EXPORT template<typename _Ty1, typename _Ty2>
bool operator!=(const Ref<_Ty1>& left, _Ty2* right) noexcept
{
    return left.Raw() != right;
}

INTRICATE_EXPORT template<typename _Ty1, typename _Ty2>
bool operator!=(_Ty1* left, const Ref<_Ty2>& right) noexcept
{
    return left != right.Raw();
}

INTRICATE_EXPORT template<typename _Ty>
bool operator!=(const Ref<_Ty>& left, std::nullptr_t) noexcept
{
    return left.Raw() != nullptr;
}

INTRICATE_EXPORT template<typename _Ty>
bool operator!=(std::nullptr_t, const Ref<_Ty>& right) noexcept
{
    return nullptr != right.Raw();
}

INTRICATE_EXPORT template<typename _Ty1, typename _Ty2>
bool operator<(const Ref<_Ty1>& left, const Ref<_Ty2>& right) noexcept
{
    return left.Raw() < right.Raw();
}

INTRICATE_EXPORT template<typename _Ty1, typename _Ty2>
bool operator<(const Ref<_Ty1>& left, _Ty2* right) noexcept
{
    return left.Raw() < right;
}

INTRICATE_EXPORT template<typename _Ty1, typename _Ty2>
bool operator<(_Ty1* left, const Ref<_Ty2>& right) noexcept
{
    return left < right.Raw();
}

INTRICATE_EXPORT template<typename _Ty>
bool operator<(const Ref<_Ty>& left, std::nullptr_t) noexcept
{
    return left.Raw() < static_cast<_Ty*>(nullptr);
}

INTRICATE_EXPORT template<typename _Ty>
bool operator<(std::nullptr_t, const Ref<_Ty>& right) noexcept
{
    return static_cast<_Ty*>(nullptr) < right.Raw();
}

INTRICATE_EXPORT template<typename _Ty1, typename _Ty2>
bool operator<=(const Ref<_Ty1>& left, const Ref<_Ty2>& right) noexcept
{
    return left.Raw() <= right.Raw();
}

INTRICATE_EXPORT template<typename _Ty1, typename _Ty2>
bool operator<=(const Ref<_Ty1>& left, _Ty2* right) noexcept
{
    return left.Raw() <= right;
}

INTRICATE_EXPORT template<typename _Ty1, typename _Ty2>
bool operator<=(_Ty1* left, const Ref<_Ty2>& right) noexcept
{
    return left <= right.Raw();
}

INTRICATE_EXPORT template<typename _Ty>
bool operator<=(const Ref<_Ty>& left, std::nullptr_t) noexcept
{
    return left.Raw() <= static_cast<_Ty*>(nullptr);
}

INTRICATE_EXPORT template<typename _Ty>
bool operator<=(std::nullptr_t, const Ref<_Ty>& right) noexcept
{
    return static_cast<_Ty*>(nullptr) <= right.Raw();
}

INTRICATE_EXPORT template<typename _Ty1, typename _Ty2>
bool operator>(const Ref<_Ty1>& left, const Ref<_Ty2>& right) noexcept
{
    return left.Raw() > right.Raw();
}

INTRICATE_EXPORT template<typename _Ty1, typename _Ty2>
bool operator>(const Ref<_Ty1>& left, _Ty2* right) noexcept
{
    return left.Raw() > right;
}

INTRICATE_EXPORT template<typename _Ty1, typename _Ty2>
bool operator>(_Ty1* left, const Ref<_Ty2>& right) noexcept
{
    return left > right.Raw();
}

INTRICATE_EXPORT template<typename _Ty>
bool operator>(const Ref<_Ty>& left, std::nullptr_t) noexcept
{
    return left.Raw() > static_cast<_Ty*>(nullptr);
}

INTRICATE_EXPORT template<typename _Ty>
bool operator>(std::nullptr_t, const Ref<_Ty>& right) noexcept
{
    return static_cast<_Ty*>(nullptr) > right.Raw();
}

INTRICATE_EXPORT template<typename _Ty1, typename _Ty2>
bool operator>=(const Ref<_Ty1>& left, const Ref<_Ty2>& right) noexcept
{
    return left.Raw() >= right.Raw();
}

INTRICATE_EXPORT template<typename _Ty1, typename _Ty2>
bool operator>=(const Ref<_Ty1>& left, _Ty2* right) noexcept
{
    return left.Raw() >= right;
}

INTRICATE_EXPORT template<typename _Ty1, typename _Ty2>
bool operator>=(_Ty1* left, const Ref<_Ty2>& right) noexcept
{
    return left >= right.Raw();
}

INTRICATE_EXPORT template<typename _Ty>
bool operator>=(const Ref<_Ty>& left, std::nullptr_t) noexcept
{
    return left.Raw() >= static_cast<_Ty*>(nullptr);
}

INTRICATE_EXPORT template<typename _Ty>
bool operator>=(std::nullptr_t, const Ref<_Ty>& right) noexcept
{
    return static_cast<_Ty*>(nullptr) >= right.Raw();
}

INTRICATE_EXPORT template<typename _Ty1, typename _Ty2>
bool operator==(const WeakRef<_Ty1>& left, const WeakRef<_Ty2>& right) noexcept
{
    return left.Raw() == right.Raw();
}

INTRICATE_EXPORT template<typename _Ty1, typename _Ty2>
bool operator==(const WeakRef<_Ty1>& left, _Ty2* right) noexcept
{
    return left.Raw() == right;
}

INTRICATE_EXPORT template<typename _Ty1, typename _Ty2>
bool operator==(_Ty1* left, const WeakRef<_Ty2> right) noexcept
{
    return left == right.Raw();
}

INTRICATE_EXPORT template<typename _Ty>
bool operator==(const WeakRef<_Ty>& left, std::nullptr_t) noexcept
{
    return left.Raw() == nullptr;
}

INTRICATE_EXPORT template<typename _Ty>
bool operator==(std::nullptr_t, const WeakRef<_Ty>& right) noexcept
{
    return nullptr == right.Raw();
}

INTRICATE_EXPORT template<typename _Ty1, typename _Ty2>
bool operator!=(const WeakRef<_Ty1>& left, const WeakRef<_Ty2>& right) noexcept
{
    return left.Raw() != right.Raw();
}

INTRICATE_EXPORT template<typename _Ty1, typename _Ty2>
bool operator!=(const WeakRef<_Ty1>& left, _Ty2* right) noexcept
{
    return left.Raw() != right;
}

INTRICATE_EXPORT template<typename _Ty1, typename _Ty2>
bool operator!=(_Ty1* left, const WeakRef<_Ty2>& right) noexcept
{
    return left != right.Raw();
}

INTRICATE_EXPORT template<typename _Ty>
bool operator!=(const WeakRef<_Ty>& left, std::nullptr_t) noexcept
{
    return left.Raw() != nullptr;
}

INTRICATE_EXPORT template<typename _Ty>
bool operator!=(std::nullptr_t, const WeakRef<_Ty>& right) noexcept
{
    return nullptr != right.Raw();
}

INTRICATE_EXPORT template<typename _Ty1, typename _Ty2>
bool operator<(const Ref<_Ty1>& left, const WeakRef<_Ty2>& right) noexcept
{
    return left.Raw() < right.Raw();
}

INTRICATE_EXPORT template<typename _Ty1, typename _Ty2>
bool operator<(const WeakRef<_Ty1>& left, _Ty2* right) noexcept
{
    return left.Raw() < right;
}

INTRICATE_EXPORT template<typename _Ty1, typename _Ty2>
bool operator<(_Ty1* left, const WeakRef<_Ty2>& right) noexcept
{
    return left < right.Raw();
}

INTRICATE_EXPORT template<typename _Ty>
bool operator<(const WeakRef<_Ty>& left, std::nullptr_t) noexcept
{
    return left.Raw() < static_cast<_Ty*>(nullptr);
}

INTRICATE_EXPORT template<typename _Ty>
bool operator<(std::nullptr_t, const WeakRef<_Ty>& right) noexcept
{
    return static_cast<_Ty*>(nullptr) < right.Raw();
}

INTRICATE_EXPORT template<typename _Ty1, typename _Ty2>
bool operator<=(const WeakRef<_Ty1>& left, const WeakRef<_Ty2>& right) noexcept
{
    return left.Raw() <= right.Raw();
}

INTRICATE_EXPORT template<typename _Ty1, typename _Ty2>
bool operator<=(const WeakRef<_Ty1>& left, _Ty2* right) noexcept
{
    return left.Raw() <= right;
}

INTRICATE_EXPORT template<typename _Ty1, typename _Ty2>
bool operator<=(_Ty1* left, const WeakRef<_Ty2>& right) noexcept
{
    return left <= right.Raw();
}

INTRICATE_EXPORT template<typename _Ty>
bool operator<=(const WeakRef<_Ty>& left, std::nullptr_t) noexcept
{
    return left.Raw() <= static_cast<_Ty*>(nullptr);
}

INTRICATE_EXPORT template<typename _Ty>
bool operator<=(std::nullptr_t, const WeakRef<_Ty>& right) noexcept
{
    return static_cast<_Ty*>(nullptr) <= right.Raw();
}

INTRICATE_EXPORT template<typename _Ty1, typename _Ty2>
bool operator>(const WeakRef<_Ty1>& left, const WeakRef<_Ty2>& right) noexcept
{
    return left.Raw() > right.Raw();
}

INTRICATE_EXPORT template<typename _Ty1, typename _Ty2>
bool operator>(const WeakRef<_Ty1>& left, _Ty2* right) noexcept
{
    return left.Raw() > right;
}

INTRICATE_EXPORT template<typename _Ty1, typename _Ty2>
bool operator>(_Ty1* left, const WeakRef<_Ty2>& right) noexcept
{
    return left > right.Raw();
}

INTRICATE_EXPORT template<typename _Ty>
bool operator>(const WeakRef<_Ty>& left, std::nullptr_t) noexcept
{
    return left.Raw() > static_cast<_Ty*>(nullptr);
}

INTRICATE_EXPORT template<typename _Ty>
bool operator>(std::nullptr_t, const WeakRef<_Ty>& right) noexcept
{
    return static_cast<_Ty*>(nullptr) > right.Raw();
}

INTRICATE_EXPORT template<typename _Ty1, typename _Ty2>
bool operator>=(const WeakRef<_Ty1>& left, const WeakRef<_Ty2>& right) noexcept
{
    return left.Raw() >= right.Raw();
}

INTRICATE_EXPORT template<typename _Ty1, typename _Ty2>
bool operator>=(const WeakRef<_Ty1>& left, _Ty2* right) noexcept
{
    return left.Raw() >= right;
}

INTRICATE_EXPORT template<typename _Ty1, typename _Ty2>
bool operator>=(_Ty1* left, const WeakRef<_Ty2>& right) noexcept
{
    return left >= right.Raw();
}

INTRICATE_EXPORT template<typename _Ty>
bool operator>=(const WeakRef<_Ty>& left, std::nullptr_t) noexcept
{
    return left.Raw() >= static_cast<_Ty*>(nullptr);
}

INTRICATE_EXPORT template<typename _Ty>
bool operator>=(std::nullptr_t, const WeakRef<_Ty>& right) noexcept
{
    return static_cast<_Ty*>(nullptr) >= right.Raw();
}

INTRICATE_NAMESPACE_END

namespace std
{
    template<class _Kty>
    struct hash;

    INTRICATE_EXPORT template<typename _Ty>
        struct hash<_INTRICATE Scope<_Ty>>
    {
        size_t operator()(const _INTRICATE Scope<_Ty>& ptr) const noexcept { return hash<_Ty*>{}(ptr.Raw()); }
    };

    INTRICATE_EXPORT template<typename _Ty>
        struct hash<_INTRICATE Ref<_Ty>>
    {
        size_t operator()(const _INTRICATE Ref<_Ty>& ptr) const noexcept { return hash<_Ty*>{}(ptr.Raw()); }
    };

    INTRICATE_EXPORT template<typename _Ty>
        struct hash<_INTRICATE WeakRef<_Ty>>
    {
        size_t operator()(const _INTRICATE WeakRef<_Ty>& ptr) const noexcept { return hash<_Ty*>{}(ptr.Raw()); }
    };
}
