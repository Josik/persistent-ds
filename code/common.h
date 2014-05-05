/*-------------------------------------------------------------*
 * common.h - header with common declarations                  *
 *-------------------------------------------------------------*
 * This file is a part of a Bachelor's thesis named Persistent *
 * data structures and their applications.                     *
 *                                                             *
 * Author: Josef Malík                                         *
 *-------------------------------------------------------------*
 */

#ifndef __COMMON_H__
#define __COMMON_H__

/* Definition of value type for structures */
typedef int VAL_TYPE;

/* Not available value */
#define NA -1

/* Type definitions for persistent deques */
typedef struct PCListElem_struct PCListElem;
typedef struct PCListBuffer_struct PCListBuffer;
typedef struct PCList_struct PCList;

/* Declarations of print procedures for persistent deques */
void printElem   (PCListElem * x);
void printBuffer (PCListBuffer * d);
void printList   (PCList       * d);
void printDeque  (PCList       * d);

/* Type definitions for persistent red-black trees */
typedef struct PRBTreeNode_struct PRBTreeNode;
typedef struct PRBTree_struct PRBTree;

/* Colour definitions for persistent red-black trees */
#define RED   0
#define BLACK 1

/* Declaration of pointer to sentinel node */
extern PRBTreeNode * const nullNode;

/* Declaration of the sentinel node */
extern PRBTreeNode sentinel;

#endif /*__COMMON_H__*/
