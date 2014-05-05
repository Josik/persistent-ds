/*----------------------------------------------------------------------------*
 * prbtreenode.h - header for nodes of confluently persistent red-black trees *
 *----------------------------------------------------------------------------*
 * This file is a part of a Bachelor's thesis named Persistent                *
 * data structures and their applications.                                    *
 *                                                                            *
 * Author: Josef Malík                                                        *
 *----------------------------------------------------------------------------*
 */

#ifndef __PRBTREENODE_H__
#define __PRBTREENODE_H__

#include "common.h"

struct PRBTreeNode_struct
{
  int                         colour;
  VAL_TYPE                    content;
  struct PRBTreeNode_struct * left;
  struct PRBTreeNode_struct * right;       
};

PRBTreeNode * makeNode  (VAL_TYPE x);
PRBTreeNode * cloneNode (PRBTreeNode * a);
void          cloneSpec (PRBTreeNode * x, PRBTreeNode * y);
int           isNull    (PRBTreeNode * a);

#endif /*__PRBTREENODE_H__*/
