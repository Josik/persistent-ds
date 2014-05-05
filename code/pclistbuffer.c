/*-------------------------------------------------------------*
 * pclistbuffer.c - implementation of buffers used in PCList   *
 *-------------------------------------------------------------*
 * This file is a part of a Bachelor's thesis named Persistent *
 * data structures and their applications.                     *
 *                                                             *
 * Author: Josef Malík                                         *
 *-------------------------------------------------------------*
 */

#include "pclistbuffer.h"
#include <stdlib.h>
#include <string.h>

/*---------------------------------------------------------------------------*/

/**
 * Creates a blank constant-sized buffer.
 */
PCListBuffer * makeBuffer (void)
{
  PCListBuffer * ret = (PCListBuffer *)malloc(sizeof(PCListBuffer));
  ret->size = 0;
  return ret;
}

/*---------------------------------------------------------------------------*/

/**
 * Corresponds to BPUSH in the thesis -- performs push of an element on a buffer.
 */
PCListBuffer * pushBuffer (PCListBuffer * a, PCListElem  * x)
{
  PCListBuffer * ret = (PCListBuffer *)malloc(sizeof(PCListBuffer));
  for (int i = 0; i < a->size; i++) setBufCont(ret, getBufCont(a, i), i + 1);
  setBufCont(ret, x, 0);
  ret->size = a->size + 1;
  return ret;
}

/*---------------------------------------------------------------------------*/

/**
 * Corresponds to BPOP in the thesis -- performs pop from a buffer.
 */
PCListBuffer * popBuffer (PCListBuffer * a, PCListElem ** x)
{
  PCListBuffer * ret;
  if (!a)
  {
    *x = NULL;
    return a;
  }
  ret = (PCListBuffer *)malloc(sizeof(PCListBuffer));
  for (int i = 1; i < a->size; i++) setBufCont(ret, getBufCont(a, i), i - 1);
  ret->size = a->size - 1;
  *x = getBufCont(a, 0);
  return ret;
}

/*---------------------------------------------------------------------------*/

/**
 * Corresponds to BINJECT in the thesis -- performs injection of an element to a buffer.
 */
PCListBuffer * injectBuffer (PCListBuffer * a, PCListElem * x)
{
  PCListBuffer * ret = (PCListBuffer *)malloc(sizeof(PCListBuffer));
  for (int i = 0; i < a->size; i++) setBufCont(ret, getBufCont(a, i), i);
  setBufCont(ret, x, a->size);
  ret->size = a->size + 1;
  return ret;
}

/*---------------------------------------------------------------------------*/

/**
 * Corresponds to BEJECT in the thesis -- performs ejection from a buffer.
 */
PCListBuffer * ejectBuffer (PCListBuffer * a, PCListElem ** x)
{
  PCListBuffer * ret;
  if (!a)
  {
    *x = NULL;
    return a;
  }
  ret = (PCListBuffer *)malloc(sizeof(PCListBuffer));
  for (int i = 0; i < a->size - 1; i++) setBufCont(ret, getBufCont(a, i), i);
  ret->size = a->size - 1;
  *x = getBufCont(a, a->size - 1);
  return ret;  
}

/*---------------------------------------------------------------------------*/

/**
 * Sets buffer's content on the given position
 */
void setBufCont (PCListBuffer * a, PCListElem * x, int pos)
{
  a->buf[pos] = x;
}

/*---------------------------------------------------------------------------*/

/**
 * Retrieves content of a buffer at the given position
 */
PCListElem * getBufCont (PCListBuffer * a, int pos)
{
  return a->buf[pos];
}

/*---------------------------------------------------------------------------*/

/**
 * Prints buffer to std. output.
 */
void printBuffer (PCListBuffer * a)
{
  if (!a) return;
  for (int i = 0; i < a->size; i++) printElem(getBufCont(a, i)); 
}

/*---------------------------------------------------------------------------*/
