/**
 * @file work.hpp
 *
 * Class representing/implementing a work design pattern.
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

#ifndef _WORK_HPP_
#define _WORK_HPP_

/*===========================================================================*\
 * system header files
\*===========================================================================*/
#include <tuple>      /* for std::tuple */
#include <functional> /* for std::function */
#include <memory>     /* for std::shared_ptr */
#include <utility>    /* for std::integer_sequence */

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

struct work_base
{
   virtual ~work_base() {};
   virtual void run() const = 0;
};

typedef std::shared_ptr<work_base> work_base_sptr;

template<typename... Args>
class work : public work_base
{
public:
    typedef std::function<void(Args...)> function;
    typedef std::tuple<Args...> arguments;

    work(const function& func, Args... args) :
       m_function(func),
       m_args(args...)
    {
    }

    ~work() override
    {
    }

    void run() const override final
    {
        func(m_args);
    }

private:
    template<std::size_t... Is>
    void func(const arguments& args, std::index_sequence<Is...>) const
    {
        m_function(std::get<Is>(args)...);
    }

    void func(const arguments& args) const
    {
        func(args, std::make_index_sequence<sizeof...(Args)>{});
    }

private:
    function  m_function;
    arguments m_args;
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

#endif /* _WORK_HPP_ */
