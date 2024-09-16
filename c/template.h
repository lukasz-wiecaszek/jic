/* SPDX-License-Identifier: MIT */
/**
 * @file template.h
 *
 * Brief description of what's this file for.
 *
 * @author Lukasz Wiecaszek <lukasz.wiecaszek@gmail.com>
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
 * global types definitions
\*===========================================================================*/
struct point
{
    int x;
    int y;
};

/*===========================================================================*\
 * static (internal linkage) functions definitions
\*===========================================================================*/
static int my_inline_function1(void)
{
    return 42;
}

/*===========================================================================*\
 * global (external linkage) objects declarations
\*===========================================================================*/
LTS_EXTERN int my_global_object;

/*===========================================================================*\
 * function forward declarations (external linkage)
\*===========================================================================*/
LTS_EXTERN void my_function1(void);
LTS_EXTERN void my_function2(const char* str);

#endif /* _TEMPLATE_H_ */
