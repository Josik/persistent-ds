/*---------------------------------------------------------------*
 * prbtree.h - header for confluently persistent red-black trees *
 *---------------------------------------------------------------*
 * This file is a part of a Bachelor's thesis named Persistent   *
 * data structures and their applications.                       *
 *                                                               *
 * Author: Josef Malík                                           *
 *---------------------------------------------------------------*
 */

#ifndef __PRBTREE_H__
#define __PRBTREE_H__

#include "common.h"
#include "prbtreenode.h"

struct PRBTree_struct
{
  PRBTreeNode * root;
};

PRBTree * makeTree     (void);
PRBTree * insert       (PRBTree * t, VAL_TYPE x);
PRBTree * erase        (PRBTree * t, VAL_TYPE x);
int       find         (PRBTree * t, VAL_TYPE x);
void      printPRBTree (PRBTreeNode * a);

#endif /*__PRBTREE_H__*/
