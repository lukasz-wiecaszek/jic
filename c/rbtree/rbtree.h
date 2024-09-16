/* SPDX-License-Identifier: MIT */
/**
 * @file rbtree.h
 *
 * This implementation of red-black tree is heavily based on
 * linux kernel's one, which on the other hand uses Wikipedia
 * (https://en.wikipedia.org/wiki/Red%E2%80%93black_tree)
 * as the base for terminology and algorithm.
 * Thus let me paste Wikipedia requirements imposed on red-black trees
 * (I replaced NIL to NULL, as this implementaion uses NULL pointer to
 * "encode the fact that there is no child node at this position
 * in the (parent) node").
 *
 * REQ1 - Every node is either red or black.
 * REQ2 - All NULL nodes are considered black.
 * REQ3 - A red node does not have a red child.
 * REQ4 - Every path from a given node to any of its descendant NULL nodes
 *        goes through the same number of black nodes.
 *
 * Legend:
 *  G - grandparent
 *  P - parent
 *  U - uncle
 *  N - node
 *  T - tmp
 *  R - red
 *  B - black
 *
 * @author Lukasz Wiecaszek <lukasz.wiecaszek@gmail.com>
 */

#ifndef _RBTREE_H_
#define _RBTREE_H_

/*===========================================================================*\
 * system header files
\*===========================================================================*/
#include <stdint.h> /* for uint64_t */

#include <stdio.h> // to be deleted

/*===========================================================================*\
 * project header files
\*===========================================================================*/

/*===========================================================================*\
 * preprocessor #define constants and macros
\*===========================================================================*/
#ifndef likely
#define likely(x) __builtin_expect(!!(x), 1)
#endif

#ifndef unlikely
#define unlikely(x) __builtin_expect(!!(x), 0)
#endif

#define RBTREE_RED   0
#define RBTREE_BLACK 1

/*===========================================================================*\
 * global types definitions
\*===========================================================================*/
struct rbtree_node {
    uint64_t parent_and_colour;
    struct rbtree_node *right;
    struct rbtree_node *left;
};

struct rbtree_root {
    struct rbtree_node *rbtree_node;
};

/*===========================================================================*\
 * static (internal linkage) functions definitions
\*===========================================================================*/
static struct rbtree_node *rbtree_parent(const struct rbtree_node *node)
{
    return (struct rbtree_node *)(node->parent_and_colour & ~1);
}

static int rbtree_colour(struct rbtree_node *node)
{
    return node->parent_and_colour & 1;
}

static void rbtree_set_parent_and_colour(
    struct rbtree_node *node, struct rbtree_node *parent, int colour)
{
    node->parent_and_colour = (uint64_t)parent + colour;
}

/*
 * Assigns new child:
 *  - to the location of old child, or
 *  - as a root of a tree if the parent is NULL.
 */
static void rbtree_change_child(
    struct rbtree_node *old, struct rbtree_node *new,
    struct rbtree_node *parent, struct rbtree_root *root)
{
    if (parent) {
        if (parent->left == old) {
            parent->left = new;
        } else {
            parent->right = new;
        }
    } else {
        root->rbtree_node = new;
    }
}

/*
 * Helper function for rotations:
 * - old's parent and color get assigned to new
 * - old gets assigned new as a parent and 'color' as a color.
 */
static void rbtree_rotate_and_set_parents(
    struct rbtree_node *old, struct rbtree_node *new,
    struct rbtree_root *root, int color)
{
    struct rbtree_node *oldsparent = rbtree_parent(old);
    new->parent_and_colour = old->parent_and_colour;
    rbtree_set_parent_and_colour(old, new, color);
    rbtree_change_child(old, new, oldsparent, root);
}

static void rbtree_rebalance(struct rbtree_root *root, struct rbtree_node *node)
{
    struct rbtree_node *grandparent;
    struct rbtree_node *parent;
    struct rbtree_node *uncle;
    struct rbtree_node *tmp;

    /*
     * Just created node (the one being passed here as @node) is initialy red,
     * thus to get pointer to the parent node it is enough to just cast
     * parent_and_colour to 'struct rbtree_node *'.
     */
    parent = (struct rbtree_node *)node->parent_and_colour;
    for (;;) {
        if (unlikely(parent == NULL)) {
            /*
             * The inserted @node shall became a root.
             * This is because of either
             *  - this is the first node,
             *  - we went through Case 1 and are no longer violating REQ3.
             * This implementation assumes that the root nodes shall be black,
             * although this is not required in the Wikipedia article.
             */
            rbtree_set_parent_and_colour(node, NULL, RBTREE_BLACK);
            break;
        }

        /* If the parent is black, we are done. */
        if (rbtree_colour(parent) == RBTREE_BLACK) {
            break;
        }

        /*
         * Otherwise, we take some corrective action as
         * per REQ3, we don't want two consecutive red nodes.
         */

        /* If the parent is red, there need to exist a black grandparent */
        grandparent = (struct rbtree_node *)parent->parent_and_colour;

        if (grandparent->right == parent) { /* grandparent->right == parent */
            uncle = grandparent->left;
            if (uncle && rbtree_colour(uncle) == RBTREE_RED) {
                /*
                 * Case 1 - node's uncle is red.
                 *
                 *       G(B)                G(R)
                 *       /  \                /  \
                 *    U(R)  P(R)    -->    U(B)  P(B)
                 *          /                   /
                 *        N(R)                N(R)
                 *
                 * However, since grandparent's parent might be red,
                 * and REQ3 does not allow this, we need to repeat the procedure
                 * at grandparent node.
                 */
                /* Swap colours. */
                uncle->parent_and_colour ^= RBTREE_BLACK;
                parent->parent_and_colour ^= RBTREE_BLACK;
                grandparent->parent_and_colour ^= RBTREE_BLACK;
                /*
                 * However, since grandparent's parent might be red,
                 * and REQ3 does not allow this, we need to repeat the procedure
                 * at grandparent node.
                 */
                node = grandparent;
                parent = (struct rbtree_node *)node->parent_and_colour;
                continue;
            }

            tmp = parent->left;
            if (node == tmp) {
                /*
                 * Case 2 - node's uncle does not exist or is black
                 * and node is the parent's left child.
                 *
                 *           G(B)                      G(B)
                 *          /   \                     /   \
                 *    U(B or X)  P(R)           U(B or X)  N(R)
                 *               /       -->                \
                 *             N(R)                          P(R)
                 *               \                          /
                 *               T(?)                      T(B)
                 *
                 * This still leaves us in violation of REQ3,
                 * the continuation into Case 3 will fix that.
                 */
                /* Right rotate at parent. */
                tmp = node->right;
                parent->left = tmp;
                node->right = parent;
                if (tmp) {
                    rbtree_set_parent_and_colour(tmp, parent, RBTREE_BLACK);
                }
                rbtree_set_parent_and_colour(parent, node, RBTREE_RED);
                parent = node;
                tmp = node->left;
            }

            /*
             * Case 3 - node's uncle does not exist or is black
             * and node is the parent's left child
             *
             *        G(B)                  P(B)
             *       /   \                 /   \
             * U(B or X) P(R)    -->     G(R)  N(R)
             *             \             /  \
             *             N(R)    U(B or X) T(B)
             */
            /* Left rotate at grandparent. */
            grandparent->right = tmp; /* == parent->rb_left */
            parent->left = grandparent;
            if (tmp) {
                rbtree_set_parent_and_colour(tmp, grandparent, RBTREE_BLACK);
            }
            rbtree_rotate_and_set_parents(grandparent, parent, root, RBTREE_RED);
            break;
        } else { /* grandparent->left == parent*/
            uncle = grandparent->right;
            if (uncle && rbtree_colour(uncle) == RBTREE_RED) {
                /*
                 * Case 1 - node's uncle is red.
                 *
                 *       G(B)                 G(R)
                 *       /  \                 /  \
                 *    P(R)  U(R)    -->    P(B)  U(B)
                 *     /                    /
                 *   N(R)                 N(R)
                 */
                /* Swap colours. */
                uncle->parent_and_colour ^= RBTREE_BLACK;
                parent->parent_and_colour ^= RBTREE_BLACK;
                grandparent->parent_and_colour ^= RBTREE_BLACK;
                /*
                 * However, since grandparent's parent might be red,
                 * and REQ3 does not allow this, we need to repeat the procedure
                 * at grandparent node.
                 */
                node = grandparent;
                parent = (struct rbtree_node *)node->parent_and_colour;
                continue;
            }

            tmp = parent->right;
            if (node == tmp) {
                /*
                 * Case 2 - node's uncle does not exist or is black
                 * and node is the parent's right child
                 *
                 *       G(B)                    G(B)
                 *      /   \                   /   \
                 *    P(R)  U(B or X)         N(R)  U(B or X)
                 *      \              -->     /
                 *      N(R)                 P(R)
                 *      /                      \
                 *    T(?)                     T(B)
                 *
                 * This still leaves us in violation of REQ3,
                 * the continuation into Case 3 will fix that.
                 */
                /* Left rotate at parent. */
                tmp = node->left;
                parent->right = tmp;
                node->left = parent;
                if (tmp) {
                    rbtree_set_parent_and_colour(tmp, parent, RBTREE_BLACK);
                }
                rbtree_set_parent_and_colour(parent, node, RBTREE_RED);
                parent = node;
                tmp = node->right;
            }

            /*
             * Case 3 - node's uncle does not exist or is black
             * and node is the parent's left child
             * (right rotate at grandparent).
             *
             *        G(B)                  P(B)
             *       /   \                 /   \
             *     P(R)  U(B or X)  -->  N(R)  G(R)
             *     / \                         /  \
             *   N(R) T(?)                   T(B) U(B or X)
             */
            /* Right rotate at grandparent. */
            grandparent->left = tmp; /* == parent->right */
            parent->right = grandparent;
            if (tmp) {
                rbtree_set_parent_and_colour(tmp, grandparent, RBTREE_BLACK);
            }
            rbtree_rotate_and_set_parents(grandparent, parent, root, RBTREE_RED);
            break;
        }
    }
}

static struct rbtree_node *rbtree_first(const struct rbtree_root *root)
{
    struct rbtree_node	*n;

    n = root->rbtree_node;
    if (!n)
        return NULL;

    while (n->left)
        n = n->left;

    return n;
}

static struct rbtree_node *rbtree_last(const struct rbtree_root *root)
{
    struct rbtree_node	*n;

    n = root->rbtree_node;
    if (!n)
        return NULL;

    while (n->right)
        n = n->right;

    return n;
}

static struct rbtree_node *rbtree_next(const struct rbtree_node *node)
{
    struct rbtree_node *parent;

    /* epmty node's parent points to its own node */
    if (node->parent_and_colour == (uint64_t)node)
        return NULL;

    /*
     * If we have a right-hand child, go down and then left as far
     * as we can.
     */
    if (node->right) {
        node = node->right;
        while (node->left)
            node = node->left;
        return (struct rbtree_node *)node;
    }

    /*
     * No right-hand children. Everything down and left is smaller than us,
     * so any 'next' node must be in the general direction of our parent.
     * Go up the tree; any time the ancestor is a right-hand child of its
     * parent, keep going up. First time it's a left-hand child of its
     * parent, said parent is our 'next' node.
     */
    while ((parent = rbtree_parent(node)) && node == parent->right)
        node = parent;

    return parent;
}

static struct rbtree_node *rbtree_prev(const struct rbtree_node *node)
{
    struct rbtree_node *parent;

    /* epmty node's parent points to its own node */
    if (node->parent_and_colour == (uint64_t)node)
        return NULL;

    /*
     * If we have a left-hand child, go down and then right as far
     * as we can.
     */
    if (node->left) {
        node = node->left;
        while (node->right)
            node = node->right;
        return (struct rbtree_node *)node;
    }

    /*
     * No left-hand children. Go up till we find an ancestor which
     * is a right-hand child of its parent.
     */
    while ((parent = rbtree_parent(node)) && node == parent->left)
        node = parent;

    return parent;
}

/*===========================================================================*\
 * global (external linkage) objects declarations
\*===========================================================================*/

/*===========================================================================*\
 * function forward declarations (external linkage)
\*===========================================================================*/

#endif /* _RBTREE_H_ */
