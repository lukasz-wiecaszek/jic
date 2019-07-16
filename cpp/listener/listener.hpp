/**
 * @file listener.hpp
 *
 * Introduces base 'listener' type and 'listeners' container class.
 *
 * This header file introduces 'listener' type which is meant as tagging type,
 * which every listener class must inherit from.
 * It also provides a 'listeners' type, which is a container for listeners
 * of one type and which provides means to call listener's specific method
 * for all currently registered/contained listeners.
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

#ifndef _LISTENER_HPP_
#define _LISTENER_HPP_

/*===========================================================================*\
 * system header files
\*===========================================================================*/
#include <type_traits>
#include <mutex>
#include <set>

/*===========================================================================*\
 * project header files
\*===========================================================================*/

/*===========================================================================*\
 * preprocessor #define constants and macros
\*===========================================================================*/

/*===========================================================================*\
 * global type definitions
\*===========================================================================*/
namespace ymn
{

/* All listeners must extend this tagging type */
struct listener
{
   virtual ~listener() = default;
};

template <class ListenerT>
class listeners
{
   static_assert(std::is_base_of<listener, ListenerT>::value,
      "ListenerT must inherit from listener");

public:
   listeners() :
      m_mutex(),
      m_listeners()
   {
   }

   ~listeners() = default;

   listeners(const listeners&) = delete;
   listeners(listeners&&) = delete;
   listeners& operator=(const listeners&) = delete;
   listeners& operator=(listeners&&) = delete;

   // returns true if added, i.e. did not exist
   bool add(ListenerT &listener)
   {
      std::lock_guard<decltype(m_mutex)> lock(m_mutex);
      return m_listeners.insert(&listener).second;
   }

   // returns true if removed, i.e. did exist
   bool remove(ListenerT &listener)
   {
      std::lock_guard<decltype(m_mutex)> lock(m_mutex);
      return (1 == m_listeners.erase(&listener));
   }

   // call function for all registered listeners,
   // e.g. call(std::bind(&listener::method, _1, arg1, arg2))
   template<class F>
   void call(F f) const
   {
      std::lock_guard<decltype(m_mutex)> lock(m_mutex);
      for (auto listener : m_listeners)
          f(listener);
   }

private:
   mutable std::mutex m_mutex;
   std::set<ListenerT*> m_listeners;
};

} /* end of namespace ymn */

/*===========================================================================*\
 * inline function/variable definitions
\*===========================================================================*/
namespace ymn
{

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

#endif /* _LISTENER_HPP_ */
