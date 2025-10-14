/**
 * @file shared_ptr.hpp
 *
 * The api is a subset of that exposed by std:shared_ptr.
 *
 * This implementation do not distinguish between managed and stored pointers
 * (there is no aliasing constructor).
 *
 * @author Lukasz Wiecaszek <lukasz.wiecaszek@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 */

#ifndef _SHARED_PTR_HPP_
#define _SHARED_PTR_HPP_

/*===========================================================================*\
 * system header files
\*===========================================================================*/
#include <type_traits>
#include <atomic>
#include <string>
#include <sstream>
#include <functional>

#if defined(DEBUG_SHARED_PTR)
#include <iostream>
#endif

/*===========================================================================*\
 * project header files
\*===========================================================================*/

/*===========================================================================*\
 * preprocessor #define constants and macros
\*===========================================================================*/

/*===========================================================================*\
 * global type definitions
\*===========================================================================*/
namespace lts
{

template<typename T>
class ref_base /* smells like android, isn't it? */
{
    //static_assert(std::is_base_of<ref_base<T>, T>::value, "T must inherit from ref_base<T> - CRTP");

public:
    void get()
    {
        m_refcount.fetch_add(1, std::memory_order_relaxed);
    }

    void put()
    {
        if (m_refcount.fetch_sub(1, std::memory_order_release) == 1) {
            std::atomic_thread_fence(std::memory_order_acquire);
            if (m_deleter)
                m_deleter(static_cast<T*>(this));
            else
               delete static_cast<T*>(this);
        }
    }

    long use_count() const
    {
        return m_refcount.load(std::memory_order_relaxed);
    }

    void set_deleter(std::function<void(T*)> deleter)
    {
       m_deleter = deleter;
    }

protected:
     ref_base() :
        m_refcount{},
        m_deleter{}
    {
        m_refcount.store(1, std::memory_order_relaxed);
    }

    virtual ~ref_base() = default;

private:
    std::atomic<long> m_refcount;
    std::function<void(T*)> m_deleter;
};

template<typename T>
class shared_ptr
{
    template<typename T1>
    friend class shared_ptr;

public:
    typedef T element_type;

    /* Constructs a shared_ptr with no managed object, i.e. epmty shared_ptr */

    constexpr shared_ptr() :
        m_ptr{nullptr}
    {
#if defined(DEBUG_SHARED_PTR)
        std::cout << __PRETTY_FUNCTION__ << std::endl;
#endif
    }

    constexpr shared_ptr(std::nullptr_t) :
        m_ptr{nullptr}
    {
#if defined(DEBUG_SHARED_PTR)
        std::cout << __PRETTY_FUNCTION__ << std::endl;
#endif
    }

    /* Constructs a shared_ptr from a T derived types */

    shared_ptr(T* ptr) :
        m_ptr{ptr}
    {
#if defined(DEBUG_SHARED_PTR)
        std::cout << __PRETTY_FUNCTION__ << std::endl;
#endif
    }

    template<typename D>
    shared_ptr(T* ptr, D deleter) :
        m_ptr{ptr}
    {
#if defined(DEBUG_SHARED_PTR)
        std::cout << __PRETTY_FUNCTION__ << std::endl;
#endif
        if (m_ptr)
            m_ptr->set_deleter(deleter);
    }

    shared_ptr(const shared_ptr<T>& other) :
        m_ptr{other.m_ptr}
    {
#if defined(DEBUG_SHARED_PTR)
        std::cout << __PRETTY_FUNCTION__ << std::endl;
#endif
        if (m_ptr)
            m_ptr->get();
    }

    shared_ptr(shared_ptr<T>&& other) :
        m_ptr{other.m_ptr}
    {
#if defined(DEBUG_SHARED_PTR)
        std::cout << __PRETTY_FUNCTION__ << std::endl;
#endif
        other.m_ptr = nullptr;
    }

    /* Constructs a shared_ptr from a U derived types, where U* must be convertible to T* */

    template<typename U>
    shared_ptr(U* ptr) :
        m_ptr{ptr}
    {
        static_assert(std::is_convertible<U*, T*>::value, "U* is not convertible to T*");
#if defined(DEBUG_SHARED_PTR)
        std::cout << __PRETTY_FUNCTION__ << std::endl;
#endif
    }

    template<typename U, typename D>
    shared_ptr(U* ptr, D deleter) :
        m_ptr{ptr}
    {
        static_assert(std::is_convertible<U*, T*>::value, "U* is not convertible to T*");
#if defined(DEBUG_SHARED_PTR)
        std::cout << __PRETTY_FUNCTION__ << std::endl;
#endif
        if (m_ptr)
            m_ptr->set_deleter(deleter);
    }

    template<typename U>
    shared_ptr(const shared_ptr<U>& other) :
       m_ptr{other.m_ptr}
    {
        static_assert(std::is_convertible<U*, T*>::value, "U* is not convertible to T*");
#if defined(DEBUG_SHARED_PTR)
        std::cout << __PRETTY_FUNCTION__ << std::endl;
#endif
        if (m_ptr)
            m_ptr->get();
    }

    template<typename U>
    shared_ptr(shared_ptr<U>&& other) :
       m_ptr{other.m_ptr}
    {
        static_assert(std::is_convertible<U*, T*>::value, "U* is not convertible to T*");
#if defined(DEBUG_SHARED_PTR)
        std::cout << __PRETTY_FUNCTION__ << std::endl;
#endif
        other.m_ptr = nullptr;
    }

    /* Destructor */

    ~shared_ptr()
    {
#if defined(DEBUG_SHARED_PTR)
        std::cout << __PRETTY_FUNCTION__ << std::endl;
#endif
        if (m_ptr)
            m_ptr->put();
    }

    /* Assigments from a T and shared_ptr<T> */

    shared_ptr& operator=(T* other)
    {
#if defined(DEBUG_SHARED_PTR)
        std::cout << __PRETTY_FUNCTION__ << std::endl;
#endif
        if (m_ptr != other) {
            if (m_ptr)
                m_ptr->put();
            m_ptr = other;
        }
        return *this;
    }

    shared_ptr& operator=(const shared_ptr<T>& other)
    {
#if defined(DEBUG_SHARED_PTR)
       std::cout << __PRETTY_FUNCTION__ << std::endl;
#endif
        if (m_ptr != other.m_ptr) {
            if (m_ptr)
                m_ptr->put();
            m_ptr = other.m_ptr;
            if (m_ptr)
                m_ptr->get();
        }
        return *this;
    }

    shared_ptr& operator=(shared_ptr<T>&& other)
    {
#if defined(DEBUG_SHARED_PTR)
       std::cout << __PRETTY_FUNCTION__ << std::endl;
#endif
        if (m_ptr != other.m_ptr) {
            if (m_ptr)
                m_ptr->put();
            m_ptr = other.m_ptr;
            other.m_ptr = nullptr;
        }
        return *this;
    }

    /* Assigments from a U and shared_ptr<U>, where U* must be convertible to T* */

    template<typename U>
    shared_ptr& operator=(U* other)
    {
#if defined(DEBUG_SHARED_PTR)
       std::cout << __PRETTY_FUNCTION__ << std::endl;
#endif
        if (m_ptr != other) {
            if (m_ptr)
                m_ptr->put();
            m_ptr = other;
        }
        return *this;
    }

    template<typename U>
    shared_ptr& operator=(const shared_ptr<U>& other)
    {
#if defined(DEBUG_SHARED_PTR)
       std::cout << __PRETTY_FUNCTION__ << std::endl;
#endif
       if (m_ptr != other.m_ptr) {
           if (m_ptr)
               m_ptr->put();
           m_ptr = other.m_ptr;
           if (m_ptr)
               m_ptr->get();
       }
       return *this;
    }

    template<typename U>
    shared_ptr& operator=(shared_ptr<U>&& other)
    {
#if defined(DEBUG_SHARED_PTR)
       std::cout << __PRETTY_FUNCTION__ << std::endl;
#endif
       if (m_ptr != other.m_ptr) {
           if (m_ptr)
               m_ptr->put();
           m_ptr = other.m_ptr;
           other.m_ptr = nullptr;
       }
       return *this;
    }

    /* Reset methods */

    void reset()
    {
#if defined(DEBUG_SHARED_PTR)
        std::cout << __PRETTY_FUNCTION__ << std::endl;
#endif
        if (m_ptr) {
            m_ptr->put();
            m_ptr = nullptr;
        }
    }

    template<typename U>
    void reset(U* ptr)
    {
        static_assert(std::is_convertible<U*, T*>::value, "U* is not convertible to T*");
#if defined(DEBUG_SHARED_PTR)
        std::cout << __PRETTY_FUNCTION__ << std::endl;
#endif
        if (m_ptr) {
            m_ptr->put();
            m_ptr = nullptr;
        }

        m_ptr = ptr;
    }

    template<typename U, typename D>
    void reset(U* ptr, D deleter)
    {
        static_assert(std::is_convertible<U*, T*>::value, "U* is not convertible to T*");
#if defined(DEBUG_SHARED_PTR)
        std::cout << __PRETTY_FUNCTION__ << std::endl;
#endif
        if (m_ptr) {
            m_ptr->put();
            m_ptr = nullptr;
        }

        m_ptr = ptr;
        if (m_ptr)
            m_ptr->set_deleter(deleter);
    }

    /* Swap */

    void swap(shared_ptr &other)
    {
#if defined(DEBUG_SHARED_PTR)
        std::cout << __PRETTY_FUNCTION__ << std::endl;
#endif
        if (m_ptr != other.m_ptr) {
             T *tmp_ptr = m_ptr;
             m_ptr = other.m_ptr;
             other.m_ptr = tmp_ptr;
        }
    }

    /* Observers */

    T* get() const
    {
        return m_ptr;
    }

    T& operator * () const
    {
        return *m_ptr;
    }

    T* operator -> () const
    {
        return m_ptr;
    }

    long use_count() const
    {
        return (m_ptr != nullptr) ? m_ptr->use_count() : 0;
    }

    operator bool () const
    {
        return m_ptr != nullptr;
    }

    std::string to_string() const
    {
        std::ostringstream stream;

        stream << "shared_ptr@";
        stream << std::hex << this;
        stream << " [use_count: ";
        stream << std::dec << use_count();
        stream << "]";

        return stream.str();
    }

    operator std::string () const
    {
        return to_string();
    }

private:
    T* m_ptr;
};

} /* end of namespace lts */

/*===========================================================================*\
 * inline function/variable definitions
\*===========================================================================*/
namespace lts
{

} /* end of namespace lts */

/*===========================================================================*\
 * global object declarations
\*===========================================================================*/
namespace lts
{

} /* end of namespace lts */

/*===========================================================================*\
 * function forward declarations
\*===========================================================================*/
namespace lts
{

} /* end of namespace lts */

#endif /* _SHARED_PTR_HPP_ */
