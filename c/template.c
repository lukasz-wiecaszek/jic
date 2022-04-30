/**
 * @file template.c
 *
 * Brief description of what's inside this source file.
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
 * local type definitions
\*===========================================================================*/
struct my_local_type
{
    int a;
    int b;
};

/*===========================================================================*\
 * global (external linkage) object definitions
\*===========================================================================*/
int my_global_object;

/*===========================================================================*\
 * local (internal linkage) function declarations
\*===========================================================================*/
static void my_local_function1(void);
static void my_local_function2(const char* str);

/*===========================================================================*\
 * local (internal linkage) object definitions
\*===========================================================================*/
static int my_local_object1;
static int my_local_object2 = 42;

/*===========================================================================*\
 * static inline (internal linkage) function definitions
\*===========================================================================*/
static inline int my_inline_function2(void)
{
    return 42;
}

/*===========================================================================*\
 * global (external linkage) function definitions
\*===========================================================================*/
void my_function1(void)
{
}

void my_function2(const char* str)
{
}

/*===========================================================================*\
 * local (internal linkage) function definitions
\*===========================================================================*/
static void my_local_function1(void)
{
}

static void my_local_function2(const char* str)
{
}
