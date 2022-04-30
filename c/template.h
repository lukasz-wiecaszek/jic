/**
 * @file template.h
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

#ifndef _TEMPLATE_H_
#define _TEMPLATE_H_

#if defined(__cplusplus)
    #define LTS_EXTERN extern "C"
#else
    #define LTS_EXTERN extern
#endif

/*===========================================================================*\
 * system header files
\*===========================================================================*/
#include <stdint.h>
#include <string.h>

/*===========================================================================*\
 * project header files
\*===========================================================================*/
#include <my_project_specific_header_file.h>

/*===========================================================================*\
 * preprocessor #define constants and macros
\*===========================================================================*/
#define MY_PREPROCESSOR_CONSTANT 42
#define MY_PREPROCESSOR_MACRO(x) ((x) + 42)

/*===========================================================================*\
 * global type definitions
\*===========================================================================*/
struct point
{
    int x;
    int y;
};

/*===========================================================================*\
 * static inline (internal linkage) function definitions
\*===========================================================================*/
static inline int my_inline_function1(void)
{
    return 42;
}

/*===========================================================================*\
 * global (external linkage) object declarations
\*===========================================================================*/
LTS_EXTERN int my_global_object;

/*===========================================================================*\
 * function forward declarations (external linkage)
\*===========================================================================*/
LTS_EXTERN void my_function1(void);
LTS_EXTERN void my_function2(const char* str);

#endif /* _TEMPLATE_H_ */
