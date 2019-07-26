/**
 * @file template.hpp
 *
 * Brief description of what's this file for.
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

#ifndef _TEMPLATE_HPP_
#define _TEMPLATE_HPP_

/*===========================================================================*\
 * system header files
\*===========================================================================*/
#include <iostream>
#include <string>

#include <cstddef>
#include <cmath>

/*===========================================================================*\
 * project header files
\*===========================================================================*/
#include <my_project_specific_header_file.hpp>

/*===========================================================================*\
 * preprocessor #define constants and macros
\*===========================================================================*/
#define MY_PREPROCESSOR_CONSTANT 42
#define MY_PREPROCESSOR_MACRO(x) ((x) + 42)

/*===========================================================================*\
 * global type definitions
\*===========================================================================*/
namespace ymn
{

template<typename T>
class class_a :
    public    class_b1,
    protected class_b2,
    private   class_b3
{
public:
    class_a(T member1, T member2, T member3) :
        class_b1{},
        class_b2{},
        class_b3{},
        m_member1{member1},
        m_member2{member2},
        m_member3{member3}
    {
    }

    ~class_a();

protected:
    void dummy_method();

private:
    T m_member1;
    T m_member2;
    T m_member3;
};

} /* end of namespace ymn */

/*===========================================================================*\
 * inline function/variable definitions
\*===========================================================================*/
namespace ymn
{

static inline void my_inline_function_definition()
{
    return 42;
}

} /* end of namespace ymn */

/*===========================================================================*\
 * global object declarations
\*===========================================================================*/
namespace ymn
{

extern int my_global_object;

} /* end of namespace ymn */

/*===========================================================================*\
 * function forward declarations
\*===========================================================================*/
namespace ymn
{

void my_function_declaration1();
void my_function_declaration2(const std::string& str);
void my_function_declaration3(const std::string* str);

} /* end of namespace ymn */

#endif /* _TEMPLATE_HPP_ */
