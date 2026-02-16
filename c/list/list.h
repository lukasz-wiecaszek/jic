/* SPDX-License-Identifier: MIT */
/**
 * @file list.h
 *
 * Header only implementation of the doubly linked lists.
 *
 * @author Lukasz Wiecaszek <lukasz.wiecaszek@gmail.com>
 */

#ifndef _LIST_H_
#define _LIST_H_

#if defined(__cplusplus)
    #define LTS_EXTERN extern "C"
#else
    #define LTS_EXTERN extern
#endif

/*===========================================================================*\
 * system header files
\*===========================================================================*/
#include <stddef.h> /* for offsetof() */

/*===========================================================================*\
 * project header files
\*===========================================================================*/

/*===========================================================================*\
 * preprocessor #define constants and macros
\*===========================================================================*/
#define list_entry(ptr, type, member) ({                  \
    const typeof(((type *)0)->member) *__mptr = (ptr);    \
    (type *)((char *)__mptr - offsetof(type, member));})

#define list_first_entry(ptr, type, member) \
    list_entry((ptr)->next, type, member)

#define list_last_entry(ptr, type, member) \
    list_entry((ptr)->prev, type, member)

#define list_for_each_forward(head, pos) \
    for (pos = (head)->next; pos != (struct list_element*)(head); pos = pos->next)

#define list_for_each_backward(head, pos) \
    for (pos = (head)->prev; pos != (struct list_element*)(head); pos = pos->prev)

/*===========================================================================*\
 * global type definitions
\*===========================================================================*/
struct list_element
{
    struct list_element* next;
    struct list_element* prev;
};

struct list_head
{
    struct list_element* next;
    struct list_element* prev;
};

/*===========================================================================*\
 * static (internal linkage) function definitions
\*===========================================================================*/
/**
 * @brief Initialize a list.
 *
 * @param head Pointer to the head of the list.
 */
static void list_init(struct list_head* head)
{
    head->next = (struct list_element*)head;
    head->prev = (struct list_element*)head;
}

/**
 * @brief Inserts a new element between two known consecutive list elements.
 *
 * @param prev Element which will be placed before the element to be added.
 * @param next Element which will be placed after the element to be added.
 * @param elem Element to be added.
 */
static void __list_add__(struct list_element* prev, struct list_element* next, struct list_element* elem)
{
    elem->next = next;
    elem->prev = prev;
    next->prev = elem;
    prev->next = elem;
}

/**
 * @brief Inserts a new element just after the specified head
 *
 * @param head List head
 * @param elem New element to be added
 */
static void list_add_head(struct list_head* head, struct list_element* elem)
{
    __list_add__((struct list_element*)head, head->next, elem);
}

/**
 * @brief Inserts a new element at the end of the specified list
 *
 * @param head List head
 * @param elem New element to be added
 */
static void list_add_tail(struct list_head* head, struct list_element* elem)
{
    __list_add__(head->prev, (struct list_element*)head, elem);
}

/**
 * @brief Deletes a list element by making the prev/next elements point to each other
 */
static void __list_del__(struct list_element* prev, struct list_element* next)
{
    next->prev = prev;
    prev->next = next;
}

/**
 * @brief Deletes an element from the list
 *
 * @param elem The element to be deleted from the list
 */
static void list_del(struct list_element* elem)
{
    __list_del__(elem->prev, elem->next);
}

/**
 * @brief Tests whether a list is empty
 *
 * @param head The list to be tested
 */
static bool list_empty(struct list_head* head)
{
    return head->next == (struct list_element*)head;
}

/*===========================================================================*\
 * global (external linkage) object declarations
\*===========================================================================*/

/*===========================================================================*\
 * function forward declarations (external linkage)
\*===========================================================================*/

#endif /* _LIST_H_ */
