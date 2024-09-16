/* SPDX-License-Identifier: MIT */
/**
 * @file template.c
 *
 * Brief description of what's inside this source file.
 *
 * @author Lukasz Wiecaszek <lukasz.wiecaszek@gmail.com>
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
 * local types definitions
\*===========================================================================*/
struct my_local_type
{
    int a;
    int b;
};

/*===========================================================================*\
 * local (internal linkage) objects definitions
\*===========================================================================*/
static int my_local_object1;
static int my_local_object2 = 42;

/*===========================================================================*\
 * global (external linkage) objects definitions
\*===========================================================================*/
int my_global_object;

/*===========================================================================*\
 * local (internal linkage) functions definitions
\*===========================================================================*/
static void my_local_function1(void)
{
}

static void my_local_function2(const char* str)
{
}

/*===========================================================================*\
 * global (external linkage) functions definitions
\*===========================================================================*/
void my_function1(void)
{
}

void my_function2(const char* str)
{
}
