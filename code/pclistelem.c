/*-------------------------------------------------------------*
 * pclistelem.c - implementation of elements used in PCList    *
 *-------------------------------------------------------------*
 * This file is a part of a Bachelor's thesis named Persistent *
 * data structures and their applications.                     *
 *                                                             *
 * Author: Josef Malík                                         *
 *-------------------------------------------------------------*
 */

#include "pclistelem.h"
#include <stdio.h>
#include <stdlib.h>

/*---------------------------------------------------------------------------*/

/** 
 * Creates a single terminal deque element with given val.
 */
PCListElem * makeElem (VAL_TYPE val)
{
  PCListElem * ret = (PCListElem *)malloc(sizeof(PCListElem));
  ret->fmb = NULL;
  ret->rl = NULL;
  ret->lmb = NULL;
  ret->val = val;
  ret->is_triple = 0;
}

/*---------------------------------------------------------------------------*/

/**
 * Creates a triple containing given fields.
 */
PCListElem * makeTriple (PCListBuffer * fmb, PCList * rl, PCListBuffer * lmb)
{
  PCListElem * ret = (PCListElem *)malloc(sizeof(PCListElem));
  ret->fmb = fmb;
  ret->rl = rl;
  ret->lmb = lmb;
  ret->val = NA;
  ret->is_triple = 1;
}

/*---------------------------------------------------------------------------*/

/**
 * Prints element to std. output (triples are printed recursively).
 */
void printElem (PCListElem * x)
{
  if (!x) return;
  if (!x->is_triple) printf("%d ", x->val);
  else
  {
    printBuffer(x->fmb);
    printList(x->rl);
    printBuffer(x->lmb);
  }
}

/*---------------------------------------------------------------------------*/
