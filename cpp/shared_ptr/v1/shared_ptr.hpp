/**
 * @file shared_ptr.hpp
 *
 * The api is a subset of that exposed by std:shared_ptr.
 *
 * The biggest (in my opinion) difference to std:shared_ptr is that you can specify
 * whether you want (or not) to have thread-safe access to the shared_ptr's managed object.
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
 * inline function definitions
\*===========================================================================*/
namespace ymn
{

} /* end of namespace ymn */

/*===========================================================================*\
 * global type definitions
\*===========================================================================*/
namespace ymn
{

template<typename T>
class shared_ptr
{
    template<typename T1>
    friend class shared_ptr;

public:
    typedef T element_type;

    /* Constructs a shared_ptr with no managed object, i.e. epmty shared_ptr */

    constexpr shared_ptr() :
        m_ptr(nullptr),
        m_control_block(nullptr)
    {
#if defined(DEBUG_SHARED_PTR)
        std::cout << __PRETTY_FUNCTION__ << std::endl;
#endif
    }

    constexpr shared_ptr(std::nullptr_t) :
        m_ptr(nullptr),
        m_control_block(nullptr)
    {
#if defined(DEBUG_SHARED_PTR)
        std::cout << __PRETTY_FUNCTION__ << std::endl;
#endif
    }

    /* Constructs a shared_ptr from a T derived types */

    shared_ptr(T* ptr) :
        m_ptr(nullptr),
        m_control_block(nullptr)
    {
#if defined(DEBUG_SHARED_PTR)
        std::cout << __PRETTY_FUNCTION__ << std::endl;
#endif
        set(ptr, &shared_ptr::builtin_deleter<T>);
    }

    template<typename D>
    shared_ptr(T* ptr, D deleter) :
        m_ptr(nullptr),
        m_control_block(nullptr)
    {
#if defined(DEBUG_SHARED_PTR)
        std::cout << __PRETTY_FUNCTION__ << std::endl;
#endif
        set(ptr, deleter);
    }

    shared_ptr(const shared_ptr<T>& other) :
        m_ptr(nullptr),
        m_control_block(nullptr)
    {
#if defined(DEBUG_SHARED_PTR)
        std::cout << __PRETTY_FUNCTION__ << std::endl;
#endif
        if (m_ptr != other.m_ptr) {
            unlink();
            link(other);
        }
    }

    shared_ptr(shared_ptr<T>&& other) :
        m_ptr(other.m_ptr),
        m_control_block(other.m_control_block)
    {
#if defined(DEBUG_SHARED_PTR)
        std::cout << __PRETTY_FUNCTION__ << std::endl;
#endif
        other.m_ptr = nullptr;
        other.m_control_block = nullptr;
    }

    /* Constructs a shared_ptr from a U derived types, where U* must be convertible to T* */

    template<typename U>
    shared_ptr(U* ptr) :
        m_ptr(nullptr),
        m_control_block(nullptr)
    {
        static_assert(std::is_convertible<U*, T*>::value, "U* is not convertible to T*");
#if defined(DEBUG_SHARED_PTR)
        std::cout << __PRETTY_FUNCTION__ << std::endl;
#endif
        set(ptr, &shared_ptr::builtin_deleter<U>);
    }

    template<typename U, typename D>
    shared_ptr(U* ptr, D deleter) :
        m_ptr(nullptr),
        m_control_block(nullptr)
    {
        static_assert(std::is_convertible<U*, T*>::value, "U* is not convertible to T*");
#if defined(DEBUG_SHARED_PTR)
        std::cout << __PRETTY_FUNCTION__ << std::endl;
#endif
        set(ptr, deleter);
    }

    template<typename U>
    shared_ptr(const shared_ptr<U> &other) :
       m_ptr(nullptr),
       m_control_block(nullptr)
    {
        static_assert(std::is_convertible<U*, T*>::value, "U* is not convertible to T*");
#if defined(DEBUG_SHARED_PTR)
        std::cout << __PRETTY_FUNCTION__ << std::endl;
#endif
        if (m_ptr != other.m_ptr) {
            unlink();
            link(other);
        }
    }

    template<typename U>
    shared_ptr(shared_ptr<U>&& other) :
        m_ptr(other.m_ptr),
        m_control_block(reinterpret_cast<decltype(m_control_block)>(other.m_control_block))
    {
        static_assert(std::is_convertible<U*, T*>::value, "U* is not convertible to T*");
#if defined(DEBUG_SHARED_PTR)
        std::cout << __PRETTY_FUNCTION__ << std::endl;
#endif
        other.m_ptr = nullptr;
        other.m_control_block = nullptr;
    }

    /* Destructor */

    ~shared_ptr()
    {
#if defined(DEBUG_SHARED_PTR)
        std::cout << __PRETTY_FUNCTION__ << std::endl;
#endif
        unlink();
    }

    /* Assigments from a T and shared_ptr<T> */

    shared_ptr& operator=(T* other)
    {
#if defined(DEBUG_SHARED_PTR)
        std::cout << __PRETTY_FUNCTION__ << std::endl;
#endif
        if (m_ptr != other) {
            reset(other);
        }
        return *this;
    }

    shared_ptr& operator=(const shared_ptr<T>& other)
    {
#if defined(DEBUG_SHARED_PTR)
        std::cout << __PRETTY_FUNCTION__ << std::endl;
#endif
        if (m_ptr != other.m_ptr) {
            unlink();
            link(other);
        }
        return *this;
    }

    shared_ptr& operator=(shared_ptr<T>&& other)
    {
#if defined(DEBUG_SHARED_PTR)
        std::cout << __PRETTY_FUNCTION__ << std::endl;
#endif
        if (m_ptr != other.m_ptr) {
            unlink();
            m_ptr = other.m_ptr;
            m_control_block = other.m_control_block;
            other.m_ptr = nullptr;
            other.m_control_block = nullptr;
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
            reset(other);
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
            unlink();
            link(other);
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
            unlink();
            m_ptr = other.m_ptr;
            m_control_block = reinterpret_cast<decltype(m_control_block)>(other.m_control_block);
            other.m_ptr = nullptr;
            other.m_control_block = nullptr;
        }
        return *this;
    }





















    void reset()
    {
#if defined(DEBUG_SHARED_PTR)
        std::cout << __PRETTY_FUNCTION__ << std::endl;
#endif
        unlink();
    }

    template<typename U>
    void reset(U* ptr)
    {
#if defined(DEBUG_SHARED_PTR)
        std::cout << __PRETTY_FUNCTION__ << std::endl;
#endif
        unlink();
        set(ptr, &shared_ptr::builtin_deleter<U>);
    }

    template<typename U, typename D>
    void reset(U* ptr, D deleter)
    {
#if defined(DEBUG_SHARED_PTR)
        std::cout << __PRETTY_FUNCTION__ << std::endl;
#endif
        unlink();
        set(ptr, deleter);
    }

    void swap(shared_ptr &other)
    {
        if (m_ptr != other.m_ptr) {
             T *tmp_ptr = m_ptr;
             m_ptr = other.m_ptr;
             other.m_ptr = tmp_ptr;

             control_block_base *tmp_block = m_control_block;
             m_control_block = other.m_control_block;
             other.m_control_block = tmp_block;
        }
    }

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

    long int use_count() const
    {
        return (m_control_block) ? m_control_block->use_count() : 0;
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
    template<typename U>
    static void builtin_deleter(U* ptr)
    {
#if defined(DEBUG_SHARED_PTR)
        std::cout << __PRETTY_FUNCTION__ << std::endl;
#endif
        delete ptr;
    }

    class control_block_base
    {
    public:
        control_block_base() :
            m_refcount(1)
        {
#if defined(DEBUG_SHARED_PTR)
            std::cout << __PRETTY_FUNCTION__ << std::endl;
#endif
        }

        virtual ~control_block_base()
        {
#if defined(DEBUG_SHARED_PTR)
            std::cout << __PRETTY_FUNCTION__ << std::endl;
#endif
        }

        long int use_count() const
        {
            return m_refcount;
        }

        void get()
        {
            m_refcount.fetch_add(1, std::memory_order_relaxed);
        }

        void put(T* ptr)
        {
            if (m_refcount.fetch_sub(1, std::memory_order_release) == 1) {
                std::atomic_thread_fence(std::memory_order_acquire);
                deleter(ptr); /* destroy managed object */
                delete this;  /* destroy this control block */
            }
        }

        virtual void deleter(T* ptr) = 0;

    private:
        std::atomic<long> m_refcount;
    };

    template<typename U, typename D>
    class control_block : public control_block_base
    {
    public:
        control_block(D deleter) :
            control_block_base(),
            m_deleter(deleter)
        {
        }

        void deleter(T* ptr) override final
        {
            m_deleter(static_cast<U*>(ptr));
        }

    private:
        D m_deleter;
    };

    template<typename U, typename D>
    void set(U* ptr, D deleter)
    {
        m_ptr = ptr;
        m_control_block = new control_block<U, D>(deleter);
    }

    template<typename U>
    void link(const shared_ptr<U>& other)
    {
        m_ptr = other.m_ptr;
        m_control_block = reinterpret_cast<decltype(m_control_block)>(other.m_control_block);
        if (m_control_block)
            m_control_block->get();
    }

    void unlink()
    {
        if (m_control_block) {
            m_control_block->put(m_ptr);
            m_control_block = nullptr;
        }
        m_ptr = nullptr;
    }

    T* m_ptr;
    control_block_base* m_control_block;
};

} /* end of namespace ymn */

/*===========================================================================*\
 * global object declarations
\*===========================================================================*/
namespace ymn
{

} /* end of namespace ymn */

/*===========================================================================*\
 * function forward declarations
\*===========================================================================*/
namespace ymn
{

} /* end of namespace ymn */

#endif /* _SHARED_PTR_HPP_ */
