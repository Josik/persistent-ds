/*----------------------------------------------------------------------*
 * prbtree.c - implementation of confluently persistent red-black trees *
 *----------------------------------------------------------------------*
 * This file is a part of a Bachelor's thesis named Persistent          *
 * data structures and their applications.                              *
 *                                                                      *
 * Author: Josef Malík                                                  *
 *----------------------------------------------------------------------*
 */

#include "prbtree.h"
#include <stdio.h>
#include <stdlib.h>

/* List of predecessors (long enough for a tree with 2^99 nodes)*/
PRBTreeNode * pList[100];

/* Actual position in predecessors' list */
int pcnt;

/* Initialization of sentinel node representing NULL */
PRBTreeNode sentinel = {RED, NA, &sentinel, &sentinel};

/* Definition of pointer to sentinel node */
PRBTreeNode * const nullNode = &sentinel;

static PRBTree            * insertTreePers  (PRBTree * t, VAL_TYPE x, PRBTreeNode ** end);
static void                 rotateLeft      (PRBTreeNode * x, PRBTreeNode * y, PRBTreeNode * z, PRBTree * t);
static void                 rotateRight     (PRBTreeNode * x, PRBTreeNode * y, PRBTreeNode * z, PRBTree * t);
static void                 relinkOutParent (PRBTreeNode * x, PRBTreeNode * y, PRBTreeNode * z, PRBTree * t);
static inline PRBTreeNode * parent          (void);
static inline PRBTreeNode * gparent         (void);
static inline PRBTreeNode * ggparent        (void);
static inline PRBTreeNode * uncle           (void);
static inline PRBTreeNode * sibling         (PRBTreeNode * x);

/*---------------------------------------------------------------------------*/

/**
 * Creates an empty persistent red-black tree.
 */
PRBTree * makeTree (void)
{
  PRBTree * ret = (PRBTree *)malloc(sizeof(PRBTree));
  ret->root = nullNode;
  return ret;
}

/*---------------------------------------------------------------------------*/

/**
 * Returns a tree formed by inserting given value to given tree.
 */
PRBTree * insert (PRBTree * t, VAL_TYPE x)
{
  pcnt = 0;
  pList[pcnt++] = nullNode;
  if (find(t, x)) return t;
  PRBTreeNode * cur;
  PRBTree * tnew = insertTreePers(t, x, &cur);
  cur->colour = RED;
  while (cur->colour == RED && cur != tnew->root && parent()->colour == RED)
  {
    if (gparent()->left == parent())
    {
      if (!isNull(uncle()) && uncle()->colour == RED)
      {
        parent()->colour = BLACK;
        gparent()->colour = RED;
        cloneSpec(uncle(), gparent());
        uncle()->colour = BLACK;
        cur = gparent();
        pcnt -= 2;
      }
      else
      {
        if (cur == parent()->right)
        {
          rotateLeft(parent(), cur, gparent(), tnew);
          PRBTreeNode * tmp = cur;
          cur = parent(); //old parent at new position
          pList[pcnt - 1] = tmp; //new parent
        }
        else
        {
          rotateRight(gparent(), parent(), ggparent(), tnew);
          parent()->colour = BLACK;
          gparent()->colour = RED;
          //remove grandparent from predecessors
          pList[pcnt - 2] = pList[pcnt - 1];
          --pcnt;
        }
      }
    }
    else
    {
      if (!isNull(uncle()) && uncle()->colour == RED)
      {
        parent()->colour = BLACK;
        gparent()->colour = RED;
        cloneSpec(uncle(), gparent());
        uncle()->colour = BLACK;
        cur = gparent();
        pcnt -= 2;
      }
      else
      {
        if (cur == parent()->left)
        {
          rotateRight(parent(), cur, gparent(), tnew);
          PRBTreeNode * tmp = cur;
          cur = parent(); //old parent at new position
          pList[pcnt - 1] = tmp; //new parent
        }
        else
        {
          rotateLeft(gparent(), parent(), ggparent(), tnew);
          parent()->colour = BLACK;
          gparent()->colour = RED;
          //remove grandparent from predecessors
          pList[pcnt - 2] = pList[pcnt - 1];
          --pcnt;
        }
      }
    }
  }
  tnew->root->colour = BLACK;
  return tnew;
}

/*---------------------------------------------------------------------------*/

/**
 * Returns a tree formed by deleting given value from given tree.
 */
PRBTree * erase (PRBTree * t, VAL_TYPE x)
{
  pcnt = 0;
  pList[pcnt++] = nullNode;
  if (!find(t, x)) return t;
  PRBTree * tnew = makeTree();
  PRBTreeNode * cur = tnew->root = cloneNode(t->root);
  while (cur->content != x)
  {
    pList[pcnt++] = cur;
    if (x < cur->content) cur = cur->left = cloneNode(cur->left);
    else if (x > cur->content) cur = cur->right = cloneNode(cur->right);
  }
  if (!isNull(cur->left) && !isNull(cur->right))
  {
    pList[pcnt++] = cur;
    PRBTreeNode * pred = cur->left = cloneNode(cur->left);
    while (!isNull(pred->right)) 
    {
      pList[pcnt++] = pred;
      pred = pred->right = cloneNode(pred->right);
    }
    cur->content = pred->content;
    cur = pred;
  }
  PRBTreeNode * next;
  if (!isNull(cur->left)) next = cur->left = cloneNode(cur->left);
  else next = cur->right = cloneNode(cur->right);
  if (isNull(parent())) tnew->root = next;
  else
  {
    if (cur == parent()->left) parent()->left = next;
    else parent()->right = next;
  }
  if (cur->colour == BLACK)
  {
    cur = next;
    while (cur->colour == BLACK && cur != tnew->root)
    {
      if (cur == parent()->left)
      {
        if (sibling(cur)->colour == RED)
        {
          cloneSpec(sibling(cur), parent());
          sibling(cur)->colour = BLACK;
          parent()->colour = RED;
          PRBTreeNode * tmp = sibling(cur);
          rotateLeft(parent(), sibling(cur), gparent(), tnew);
          //insert new predecessor above parent
          pList[pcnt] = parent();
          pList[pcnt - 1] = tmp;
          ++pcnt;
        }
        if (sibling(cur)->left->colour == BLACK && sibling(cur)->right->colour == BLACK)
        {
          cloneSpec(sibling(cur), parent());
          sibling(cur)->colour = RED;
          cur = parent();
          --pcnt;
        }
        else 
        {
          if (sibling(cur)->right->colour == BLACK)
          {
            cloneSpec(sibling(cur), parent());
            cloneSpec(sibling(cur)->left, sibling(cur));
            sibling(cur)->left->colour = BLACK;
            sibling(cur)->colour = RED;
            rotateRight(sibling(cur), sibling(cur)->left, parent(), tnew);
          }
          cloneSpec(sibling(cur), parent());
          cloneSpec(sibling(cur)->right, sibling(cur));
          sibling(cur)->colour = parent()->colour;
          parent()->colour = BLACK;
          sibling(cur)->right->colour = BLACK;
          rotateLeft(parent(), sibling(cur), gparent(), tnew);
          cur = tnew->root;
        }
      }
      else
      {
        if (sibling(cur)->colour == RED)
        {
          cloneSpec(sibling(cur), parent());
          sibling(cur)->colour = BLACK;
          parent()->colour = RED;
          PRBTreeNode * tmp = sibling(cur);
          rotateRight(parent(), sibling(cur), gparent(), tnew);
          //insert new predecessor above parent
          pList[pcnt] = parent();
          pList[pcnt - 1] = tmp;
          ++pcnt;
        }
        if (sibling(cur)->left->colour == BLACK && sibling(cur)->right->colour == BLACK)
        {
          cloneSpec(sibling(cur), parent());
          sibling(cur)->colour = RED;
          cur = parent();
          --pcnt;
        }
        else 
        {
          if (sibling(cur)->left->colour == BLACK)
          {
            cloneSpec(sibling(cur), parent());
            cloneSpec(sibling(cur)->right, sibling(cur));
            sibling(cur)->right->colour = BLACK;
            sibling(cur)->colour = RED;
            rotateLeft(sibling(cur), sibling(cur)->right, parent(), tnew);
          }
          cloneSpec(sibling(cur), parent());
          cloneSpec(sibling(cur)->left, sibling(cur));
          sibling(cur)->colour = parent()->colour;
          parent()->colour = BLACK;
          sibling(cur)->left->colour = BLACK;
          rotateRight(parent(), sibling(cur), gparent(), tnew);
          cur = tnew->root;
        }
      }        
    }
  }
  cur->colour = BLACK;
  return tnew;
}

/*---------------------------------------------------------------------------*/

/**
 * Returns whether given tree contains certain value or not.
 */
int find (PRBTree * t, VAL_TYPE x)
{
  PRBTreeNode * cur = t->root;
  while (!isNull(cur))
  {
    if (x < cur->content) cur = cur->left;
    else if (x > cur->content) cur = cur->right;
    else return 1;
  }
  return 0;
}

/*---------------------------------------------------------------------------*/

/**
 * This function adds new element to a right place along with copying whole access path.
 * The addition is thus performed on a new copy.
 */
static PRBTree * insertTreePers (PRBTree * t, VAL_TYPE x, PRBTreeNode ** end)
{
  PRBTree * ret = makeTree();
  PRBTreeNode * cur = ret->root = cloneNode(t->root);
  PRBTreeNode * prev = nullNode;
  while (!isNull(cur))
  {
    prev = cur;
    pList[pcnt++] = cur;
    if (x < cur->content) cur = cur->left = cloneNode(cur->left);
    else if (x > cur->content) cur = cur->right = cloneNode(cur->right);
    else return t;
  }
  cur = makeNode(x);
  if (!isNull(prev))
  {
    if (x < prev->content) prev->left = cur;
    else if (x > prev->content) prev->right = cur; 
  }
  if (isNull(ret->root)) ret->root = cur; //inserting to an empty tree
  *end = cur;
  return ret;
}

/*---------------------------------------------------------------------------*/

/**
 * Prints a subtree rooted at given node (by inorder traversal).
 */
void printPRBTree (PRBTreeNode * a)
{
  if (isNull(a)) return;
  printPRBTree(a->left);
  printf("%d ", a->content);
  printPRBTree(a->right);
}

/*---------------------------------------------------------------------------*/

/**
 * Performs a left rotation of given node, its right child and its parent over given tree.
 */
static void rotateLeft (PRBTreeNode * x, PRBTreeNode * y, PRBTreeNode * z, PRBTree * t)
{
  if (!isNull(y)) x->right = y->left;
  if (!isNull(y)) y->left = x;
  relinkOutParent(x, y, z, t);
}

/*---------------------------------------------------------------------------*/

/**
 * Performs a right rotation of given node, its left child and its parent over given tree.
 */
static void rotateRight (PRBTreeNode * x, PRBTreeNode * y, PRBTreeNode * z, PRBTree * t)
{
  if (!isNull(y)) x->left = y->right;
  if (!isNull(y)) y->right = x;
  relinkOutParent(x, y, z, t);
}

/*---------------------------------------------------------------------------*/

/**
 * Links correctly rotated nodes with the parent node above the rotation.
 */
static void relinkOutParent (PRBTreeNode * x, PRBTreeNode * y, PRBTreeNode * z, PRBTree * t)
{
  if (!isNull(z))
  {
    if (x == z->left) z->left = y;
    else z->right = y;
  }
  else t->root = y;
}

/*---------------------------------------------------------------------------*/

/**
 * Returns a parent node in current status of the bottom-up traversal.
 */
static inline PRBTreeNode * parent (void)
{
  return pList[pcnt - 1];
}

/*---------------------------------------------------------------------------*/

/**
 * Returns a grandparent node in current status of the bottom-up traversal.
 */
static inline PRBTreeNode * gparent (void)
{
  return pList[pcnt - 2];
}

/*---------------------------------------------------------------------------*/

/**
 * Returns a parent of grandparent node in current status of the bottom-up traversal.
 */
static inline PRBTreeNode * ggparent (void)
{
  return pList[pcnt - 3];
}

/*---------------------------------------------------------------------------*/

/**
 * Returns an uncle node in current status of the bottom-up traversal.
 * This function assumes that grandparent node exists at that time.
 */
static inline PRBTreeNode * uncle (void)
{
  PRBTreeNode * g = gparent();
  PRBTreeNode * p = parent();
  if (g->left == p) return g->right;
  return g->left;
}

/*---------------------------------------------------------------------------*/

/**
 * Returns a sibling node of given node.
 * This function assumes that its parent node exists.
 */
static inline PRBTreeNode * sibling (PRBTreeNode * x)
{
  PRBTreeNode * p = parent();
  if (p->left == x) return p->right;
  return p->left;
}

/*---------------------------------------------------------------------------*/
