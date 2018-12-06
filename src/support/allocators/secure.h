// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2017 The Bitcoin Core developers
// Copyright (c) 2018 The Bitcoin Post-Quantum developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_SUPPORT_ALLOCATORS_SECURE_H
#define BITCOIN_SUPPORT_ALLOCATORS_SECURE_H


/*

//
// Allocator that locks its contents from being paged
// out of memory and clears its contents before deletion.
//
template <typename T>
struct secure_allocator : public std::allocator<T> {
    // MSVC8 default copy constructor is broken
    typedef std::allocator<T> base;
    typedef typename base::size_type size_type;
    typedef typename base::difference_type difference_type;
    typedef typename base::pointer pointer;
    typedef typename base::const_pointer const_pointer;
    typedef typename base::reference reference;
    typedef typename base::const_reference const_reference;
    typedef typename base::value_type value_type;
    secure_allocator() noexcept {}
    secure_allocator(const secure_allocator& a) noexcept : base(a) {}
    template <typename U>
    secure_allocator(const secure_allocator<U>& a) noexcept : base(a)
    {
    }
    ~secure_allocator() noexcept {}
    template <typename _Other>
    struct rebind {
        typedef secure_allocator<_Other> other;
    };

    T* allocate(std::size_t n, const void* hint = 0)
    {
        return static_cast<T*>(LockedPoolManager::Instance().alloc(sizeof(T) * n));
    }

    void deallocate(T* p, std::size_t n)
    {
        if (p != nullptr) {
            memory_cleanse(p, sizeof(T) * n);
        }
        LockedPoolManager::Instance().free(p);
    }
};
  
 */

#include <botan/secmem.h>
#include <support/lockedpool.h>
#include <support/cleanse.h>
#include <string>



// This is exactly like std::string, but with a custom allocator.
using Botan::secure_allocator;
typedef std::basic_string<char, std::char_traits<char>, secure_allocator<char> > SecureString;

using secure_vector = Botan::secure_vector<uint8_t>;

//template<typename T>
//struct secure_allocator : public Botan::secure_allocator<T>
//{
//public:
//    template<class U> secure_allocator(const secure_allocator<U>& other) BOTAN_NOEXCEPT;
    //secure_allocator(const secure_allocator<T>&) = default;
//};
//
//template<class U> secure_allocator(const secure_allocator<U>& other);

//using SecureString = std::basic_string<char, std::char_traits<char>, secure_allocator<char> >;


#endif // BITCOIN_SUPPORT_ALLOCATORS_SECURE_H
