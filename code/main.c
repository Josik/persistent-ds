/*---------------------------------------------------------------------*
 * main.c - example usage of implemented persistent data structures    *
 *---------------------------------------------------------------------*
 * This file is a part of a Bachelor's thesis named Persistent         *
 * data structures and their applications.                             *
 *                                                                     *
 * Author: Josef Malík                                                 *
 *---------------------------------------------------------------------*
 */

#include <stdio.h>

#include "pclist.h"
#include "prbtree.h"

/** 
 * This function prints given deque to std. output
 */
void printDeque (PCList * d)
{
  printf("[ ");
  printList(d);
  printf("]\n");
}

/** 
 * This function prints given red-black tree (inorder) to std. output
 */
void printTree (PRBTree * t)
{
  printf("[ ");
  printPRBTree(t->root);
  printf("]\n");
}

int main (int argc, char * argv [])
{
  /**
   * Example usage of confluently persistent catenable lists (deques):
   */
  /*---------------------------------------------------------------------------*/
  PCList * d = makeList(1);
  printf("Deque example usage:\n");
  for (int i = 2; i <= 3; i++) d = inject(d, i);
  printDeque(d);
  /* [ 1 2 3 ] */
  PCList * e = makeList(4);
  for (int i = 5; i <= 6; i++) e = push(e, i);
  printDeque(e);
  /* [ 6 5 4 ] */
  printDeque(catenate(e, d));
  /* [ 6 5 4 1 2 3 ] */
  int dummy;
  PCList * f = catenate(eject(e, &dummy), pop(d, &dummy));
  printDeque(f);
  /* [ 6 5 2 3 ] */
  printDeque(catenate(catenate(f, f), f));
  /* [ 6 5 2 3 6 5 2 3 6 5 2 3 ] */
  /*---------------------------------------------------------------------------*/
  
  /**
   * Example usage of confluently persistent red-black tree:
   */
  /*---------------------------------------------------------------------------*/
  PRBTree * t = makeTree();
  printf("\nRed-black tree example usage:\n");
  t = insert(t, 1);
  printTree(t);
  /* [ 1 ] */
  printTree(insert(insert(t, 3), 2));
  /* [ 1 2 3 ] */
  printTree(t);
  /* [ 1 ] */
  printTree(insert(erase(insert(t, 3), 3), 4));
  /* [ 1 4 ] */
  /*---------------------------------------------------------------------------*/
  
  return 0;
}
