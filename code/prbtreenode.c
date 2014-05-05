/*-----------------------------------------------------------------------------------*
 * prbtreenode.h - implementation of nodes of confluently persistent red-black trees *
 *-----------------------------------------------------------------------------------*
 * This file is a part of a Bachelor's thesis named Persistent                       *
 * data structures and their applications.                                           *
 *                                                                                   *
 * Author: Josef Malík                                                               *
 *-----------------------------------------------------------------------------------*
 */

#include "prbtreenode.h"
#include <stdlib.h>

/*---------------------------------------------------------------------------*/

/**
 * Creates a tree node (with given value) of persistent red-black tree.
 */
PRBTreeNode * makeNode (VAL_TYPE x)
{
  PRBTreeNode * ret = (PRBTreeNode *)malloc(sizeof(PRBTreeNode));
  ret->colour = BLACK;
  ret->content = x;
  ret->left = nullNode;
  ret->right = nullNode;
  return ret;
}

/*---------------------------------------------------------------------------*/

/**
 * Returns a copy of given node (in case it is not null leaf).
 */
PRBTreeNode * cloneNode (PRBTreeNode * x)
{
  if (isNull(x)) return nullNode;
  PRBTreeNode * ret = makeNode(x->content);
  ret->colour = x->colour;
  ret->left = x->left;
  ret->right = x->right;
  return ret;
}

/*---------------------------------------------------------------------------*/

/**
 * Makes a copy of y's child x and automatically links it with y.
 */
void cloneSpec (PRBTreeNode * x, PRBTreeNode * y)
{
  if (x == y->left) y->left = cloneNode(x);
  else y->right = cloneNode(x);
}

/*---------------------------------------------------------------------------*/

/**
 * Returns whether a node is a null leaf node.
 */
int isNull (PRBTreeNode * x)
{
  return x == nullNode; 
}

/*---------------------------------------------------------------------------*/
