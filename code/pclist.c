/*---------------------------------------------------------------------*
 * pclist.c - implementation of confluently persistent catenable lists *
 *---------------------------------------------------------------------*
 * This file is a part of a Bachelor's thesis named Persistent         *
 * data structures and their applications.                             *
 *                                                                     *
 * Author: Josef Malík                                                 *
 *---------------------------------------------------------------------*
 */

#include "pclist.h"
#include <stdio.h>
#include <stdlib.h>

static PCList * listCreate  (PCListElem * x);
static PCList * listPush    (PCList * d, PCListElem * x);
static PCList * listPop     (PCList * d, PCListElem ** x);
static PCList * listInject  (PCList * d, PCListElem * x);
static PCList * listEject   (PCList * d, PCListElem ** x);
static PCList * simplePop   (PCList * d, PCListElem ** x);
static PCList * simpleEject (PCList * d, PCListElem ** x);
static void     concatSplit (PCListBuffer * b, PCListBuffer ** b1, PCListBuffer ** b2, int first);

/*---------------------------------------------------------------------------*/

/**
 * Creates a new list(deque) with one element of a given value.
 */
PCList * makeList (VAL_TYPE val)
{
  PCListElem * x = makeElem(val);
  return listCreate(x);
}

/*---------------------------------------------------------------------------*/

/**
 * Constructs a new list(deque) containing given fields.
 */
PCList * consList (PCListBuffer * p, PCList * l, PCListBuffer * m, PCList * r, PCListBuffer * s, int so)
{
  PCList * ret = (PCList *)malloc(sizeof(PCList));
  ret->p = p;
  ret->l = l;
  ret->m = m;
  ret->r = r;
  ret->s = s;
  ret->suffix_only = so;
  return ret;
}

/*---------------------------------------------------------------------------*/

/**
 * Wrapper function for user that pushes an element to a list(deque).
 */
PCList * push (PCList * d, VAL_TYPE val)
{
  PCListElem * x = makeElem(val);
  return listPush(d, x);
}

/*---------------------------------------------------------------------------*/

/**
 * Wrapper function for user that pops an element from a list(deque).
 */
PCList * pop (PCList * d, VAL_TYPE * val)
{
  if (!d)
  {
    *val = NA;
    return d;
  }
  if (d->suffix_only && d->s->size == 1)
  {
    *val = getBufCont(d->s, 0)->val;
    return NULL;
  }
  PCListElem * x;
  PCList * ret = listPop(d, &x);
  *val = x->val;
  return ret;
}

/*---------------------------------------------------------------------------*/

/**
 * Wrapper function for user that injects an element to a list(deque).
 */
PCList * inject (PCList * d, VAL_TYPE val)
{
  PCListElem * x = makeElem(val);
  return listInject(d, x);
}

/*---------------------------------------------------------------------------*/

/**
 * Wrapper function for user that ejects an element from a list(deque).
 */
PCList * eject (PCList * d, VAL_TYPE * val)
{
  if (!d)
  {
    *val = NA;
    return d;
  }
  if (d->suffix_only && d->s->size == 1)
  {
    *val = getBufCont(d->s, 0)->val;
    return NULL;
  }
  PCListElem * x;
  PCList * ret = listEject(d, &x);
  *val = x->val;
  return ret;
}

/*---------------------------------------------------------------------------*/

/**
 * Concatenates two given deques.
 */
PCList * catenate (PCList * d1, PCList * d2)
{
  if (!d1) return d2;
  else if (!d2) return d1;
  PCList * target;
  //either of the lists is suffix only => its elements are pushed/injected to the other list
  if (!isTuple(d1))
  {
    target = d2;
    for (int i = d1->s->size - 1; i >= 0; i--) target = listPush(target, getBufCont(d1->s, i));
    return target;
  }
  else if (!isTuple(d2))
  {
    target = d1;
    for (int i = 0; i < d2->s->size; i++) target = listInject(target, getBufCont(d2->s, i));
    return target;
  }
  //both lists are tuples
  PCListBuffer * mnew = makeBuffer();
  setBufCont(mnew, getBufCont(d1->s, d1->s->size - 1), 0);
  setBufCont(mnew, getBufCont(d2->p, 0), 1);
  mnew->size = 2;
  PCListBuffer * s1, * s2;
  concatSplit(d1->s, &s1, &s2, 1);
  PCList * lnew = listInject(d1->l, makeTriple(d1->m, d1->r, s1));
  if (s2->size) lnew = listInject(lnew, makeTriple(s2, NULL, NULL));
  PCListBuffer * p1, * p2;
  concatSplit(d2->p, &p1, &p2, 0);
  PCList * rnew = listPush(d2->r, makeTriple(p2, d2->l, d2->m));
  if (p1->size) rnew = listPush(rnew, makeTriple(p1, NULL, NULL));
  return consList(d1->p, lnew, mnew, rnew, d2->s, 0);
}

/*---------------------------------------------------------------------------*/

/**
 * Returns, whether a deque is represented by a suffix only or not.
 */
int isTuple (PCList * d)
{
  return !d->suffix_only; 
}

/*---------------------------------------------------------------------------*/

/**
 * Transforms given deque in a way that its prefix is green after the operation.
 */
void gPrefix (PCList * d)
{
  if (d->p->size == 6)
  {
    PCListBuffer * pnew, * qnew;
    pnew = makeBuffer();
    qnew = makeBuffer();
    int cp = 0;
    for (int j = 0; j < 4; j++) setBufCont(pnew, getBufCont(d->p, cp++), j);
    pnew->size = 4;
    for (int j = 0; j < 2; j++) setBufCont(qnew, getBufCont(d->p, cp++), j);
    qnew->size = 2;
    d->p = pnew;
    d->l = listPush(d->l, makeTriple(qnew, NULL, NULL));
    return;
  }
  PCList * x;
  if (d->l) x = d->l;
  else x = d->r;
  PCList * xtmp;
  PCListElem * t;
  xtmp = simplePop(x, &t);
  PCListBuffer * f = t->fmb;
  int first = 1;
  if (!f)
  {
    first = 0;
    f = t->lmb;
  }
  if (f->size == 2 && !t->rl) 
  {
    xtmp = listPop(x, &t);
    if (first) f = t->fmb;
    else f = t->lmb;
  }
  PCListElem * a, * b;
  if (f->size == 3)
  {
    PCListBuffer * fnew = popBuffer(f, &a);
    PCListElem * tnew;
    if (first) tnew = makeTriple(fnew, t->rl, t->lmb);
    else tnew = makeTriple(t->fmb, t->rl, fnew);
    PCList * xnew = listPush(xtmp, tnew);
    if (d->l)
    {
      d->p = injectBuffer(d->p, a);
      d->l = xnew;
    }
    else
    {
      PCListBuffer * mtmp = popBuffer(d->m, &b);
      PCListBuffer * mnew = injectBuffer(mtmp, a);
      d->p = injectBuffer(d->p, b);
      d->m = mnew;
      d->r = xnew;
    }
  }
  else
  {
    if (d->l)
    {
      PCListBuffer * ptmp = injectBuffer(d->p, getBufCont(f, 0));
      PCListBuffer * pnew = injectBuffer(ptmp, getBufCont(f, 1));
      if (!t->rl && !(first ? t->lmb : t->fmb))
      {
        d->p = pnew;
        d->l = xtmp;
      }
      else
      {
        PCList * lnew = catenate(t->rl, listPush(xtmp, makeTriple(t->lmb, NULL, NULL)));
        d->p = pnew;
        d->l = lnew;
      }
    }
    else
    {
      PCListBuffer * ptmp = injectBuffer(d->p, getBufCont(d->m, 0));
      PCListBuffer * pnew = injectBuffer(ptmp, getBufCont(d->m, 1));
      if (!t->rl && !(first ? t->lmb : t->fmb))
      {
        d->p = pnew;
        d->m = f;
        d->r = xtmp;
      }
      else
      {
        PCList * rnew = catenate(t->rl, listPush(xtmp, makeTriple(t->lmb, NULL, NULL)));
        d->p = pnew;
        d->m = f;
        d->r = rnew;
      }
    }
  }
}

/*---------------------------------------------------------------------------*/

/**
 * Transforms given deque in a way that its suffix is green after the operation.
 */
void gSuffix (PCList * d)
{
  if (d->s->size == 6)
  {
    PCListBuffer * snew, * qnew;
    snew = makeBuffer();
    qnew = makeBuffer();
    int cp = d->s->size - 1;
    for (int j = 3; j >= 0; j--) setBufCont(snew, getBufCont(d->s, cp--), j);
    snew->size = 4;
    for (int j = 1; j >= 0; j--) setBufCont(qnew, getBufCont(d->s, cp--), j);
    qnew->size = 2;
    d->s = snew;
    d->r = listInject(d->r, makeTriple(NULL, NULL, qnew));
    return;
  }
  PCList * x;
  if (d->r) x = d->r;
  else x = d->l;
  PCList * xtmp;
  PCListElem * t;
  xtmp = simpleEject(x, &t);
  PCListBuffer * f = t->lmb;
  int first = 1;
  if (!f)
  {
    first = 0;
    f = t->fmb;
  }
  if (f->size == 2 && !t->rl) 
  {
    xtmp = listEject(x, &t);
    if (first) f = t->lmb;
    else f = t->fmb;
  }
  PCListElem * a, * b;
  if (f->size == 3)
  {
    PCListBuffer * fnew = ejectBuffer(f, &a);
    PCListElem * tnew;
    if (first) tnew = makeTriple(t->fmb, t->rl, fnew);
    else tnew = makeTriple(fnew, t->rl, t->lmb);
    PCList * xnew = listInject(xtmp, tnew);
    if (d->r)
    {
      d->s = pushBuffer(d->s, a);
      d->r = xnew;
    }
    else
    {
      PCListBuffer * mtmp = ejectBuffer(d->m, &b);
      PCListBuffer * mnew = pushBuffer(mtmp, a);
      d->s = pushBuffer(d->s, b);
      d->m = mnew;
      d->l = xnew;
    }
  }
  else
  {
    if (d->r)
    {
      PCListBuffer * stmp = pushBuffer(d->s, getBufCont(f, 1));
      PCListBuffer * snew = pushBuffer(stmp, getBufCont(f, 0));
      if (!t->rl && !(first ? t->fmb : t->lmb))
      {
        d->s = snew;
        d->r = xtmp;
      }
      else
      {
        PCList * rnew = catenate(listInject(xtmp, makeTriple(NULL, NULL, t->fmb)), t->rl);
        d->s = snew;
        d->r = rnew;
      }
    }
    else
    {
      PCListBuffer * stmp = pushBuffer(d->s, getBufCont(d->m, 1));
      PCListBuffer * snew = pushBuffer(stmp, getBufCont(d->m, 0));
      if (!t->rl && !(first ? t->fmb : t->lmb))
      {
        d->s = snew;
        d->m = f;
        d->l = xtmp;
      }
      else
      {
        PCList * lnew = catenate(listInject(xtmp, makeTriple(NULL, NULL, t->fmb)), t->rl);
        d->s = snew;
        d->m = f;
        d->l = lnew;
      }
    }
  }  
}

/*---------------------------------------------------------------------------*/

/**
 * Recursively prints a list(deque) to std. output.
 */
void printList (PCList * d)
{
  if (!d) return;
  printBuffer(d->p);
  printList(d->l);
  printBuffer(d->m);
  printList(d->r);
  printBuffer(d->s);
}

/*---------------------------------------------------------------------------*/

/**
 * Helping function to create a list with the element already as a PCListElem.
 */
static PCList * listCreate (PCListElem * x)
{
  PCList * ret = (PCList *)malloc(sizeof(PCList));
  ret->p = NULL;
  ret->l = NULL;
  ret->m = NULL;
  ret->r = NULL;
  ret->s = makeBuffer();
  setBufCont(ret->s, x, 0);
  ret->s->size = 1;
  ret->suffix_only = 1;
}

/*---------------------------------------------------------------------------*/

/**
 * Actual procedure that performs push.
 */
static PCList * listPush (PCList * d, PCListElem * x)
{
  if (!d) return listCreate(x);
  if (isTuple(d))
  {
    if (d->p->size == 6) gPrefix(d);
    return consList(pushBuffer(d->p, x), d->l, d->m, d->r, d->s, 0);
  }
  PCListBuffer * stmp = pushBuffer(d->s, x);
  if (stmp->size <= 8) return consList(NULL, NULL, NULL, NULL, stmp, 1);
  PCListBuffer * pnew, * mnew, * snew;
  pnew = makeBuffer();
  mnew = makeBuffer();
  snew = makeBuffer();
  for (int j = 0; j < 4; j++) setBufCont(pnew, getBufCont(stmp, j), j);
  pnew->size = 4;
  for (int i = 4, j = 0; j < 2; i++, j++) setBufCont(mnew, getBufCont(stmp, i), j);
  mnew->size = 2;
  for (int i = 6, j = 0; j < 3; i++, j++) setBufCont(snew, getBufCont(stmp, i), j);
  snew->size = 3;
  return consList(pnew, NULL, mnew, NULL, snew, 0);
}

/*---------------------------------------------------------------------------*/

/**
 * Actual procedure that performs pop.
 */
static PCList * listPop (PCList * d, PCListElem ** x)
{
  if (d->suffix_only || d->p->size > 3) return simplePop(d, x);
  else if (d->p->size == 3 && (d->l || d->r))
  {
    gPrefix(d);
    return simplePop(d, x);
  }
  //else for all other cases
  PCListElem * a = getBufCont(d->p, 0);
  *x = a;
  if (d->s->size == 3)
  {
    PCListBuffer * snew = makeBuffer();
    int cp = 0;
    for (int i = 1; i < d->p->size; i++) setBufCont(snew, getBufCont(d->p, i), cp++);
    for (int i = 0; i < d->m->size; i++) setBufCont(snew, getBufCont(d->m, i), cp++);
    for (int i = 0; i < d->s->size; i++) setBufCont(snew, getBufCont(d->s, i), cp++);
    snew->size = cp;
    return consList(NULL, NULL, NULL, NULL, snew, 1);
  }
  else
  {
    PCListElem * b, * c;
    PCListBuffer * pnew, * mnew, * snew, * ptmp, * mtmp;
    ptmp = popBuffer(d->p, &a);
    mtmp = popBuffer(d->m, &b);
    snew = popBuffer(d->s, &c);
    pnew = injectBuffer(ptmp, b);
    mnew = injectBuffer(mtmp, c);
    return consList(pnew, NULL, mnew, NULL, snew, 0);
  }
}

/*---------------------------------------------------------------------------*/

/**
 * Actual procedure that performs inject.
 */
static PCList * listInject (PCList * d, PCListElem * x)
{
  if (!d) return listCreate(x);
  if (isTuple(d))
  {
    if (d->s->size == 6) gSuffix(d);
    return consList(d->p, d->l, d->m, d->r, injectBuffer(d->s, x), 0);
  }
  PCListBuffer * stmp = injectBuffer(d->s, x);
  if (stmp->size <= 8) return consList(NULL, NULL, NULL, NULL, stmp, 1);
  PCListBuffer * pnew, * mnew, * snew;
  pnew = makeBuffer();
  mnew = makeBuffer();
  snew = makeBuffer();
  for (int j = 0; j < 3; j++) setBufCont(pnew, getBufCont(stmp, j), j);
  pnew->size = 3;
  for (int i = 3, j = 0; j < 2; i++, j++) setBufCont(mnew, getBufCont(stmp, i), j);
  mnew->size = 2;
  for (int i = 5, j = 0; j < 4; i++, j++) setBufCont(snew, getBufCont(stmp, i), j);
  snew->size = 4;
  return consList(pnew, NULL, mnew, NULL, snew, 0);
}

/*---------------------------------------------------------------------------*/

/**
 * Actual procedure that performs eject.
 */
static PCList * listEject (PCList * d, PCListElem ** x)
{
  if (d->suffix_only || d->s->size > 3) return simpleEject(d, x);
  else if (d->s->size == 3 && (d->l || d->r))
  {
    gSuffix(d);
    return simpleEject(d, x);
  }
  //else for all other cases
  PCListElem * a = getBufCont(d->s, d->s->size - 1);
  *x = a;
  if (d->p->size == 3)
  {
    PCListBuffer * snew = makeBuffer();
    int cp = 0;
    for (int i = 0; i < d->p->size; i++) setBufCont(snew, getBufCont(d->p, i), cp++);
    for (int i = 0; i < d->m->size; i++) setBufCont(snew, getBufCont(d->m, i), cp++);
    for (int i = 0; i < d->s->size - 1; i++) setBufCont(snew, getBufCont(d->s, i), cp++);
    snew->size = cp;
    return consList(NULL, NULL, NULL, NULL, snew, 1);
  }
  else
  {
    PCListElem * b, * c;
    PCListBuffer * snew, * mnew, * pnew, * stmp, * mtmp;
    stmp = ejectBuffer(d->s, &a);
    mtmp = ejectBuffer(d->m, &b);
    pnew = ejectBuffer(d->p, &c);
    snew = pushBuffer(stmp, b);
    mnew = pushBuffer(mtmp, c);
    return consList(pnew, NULL, mnew, NULL, snew, 0);
  } 
}

/*---------------------------------------------------------------------------*/

/**
 * Pop that doesn't check or change size properties of list(deque) under pop.
 */
static PCList * simplePop (PCList * d, PCListElem ** x)
{
  if (!d)
  {
    *x = NULL;
    return d;
  }
  if (d->suffix_only)
  {
    *x = getBufCont(d->s, 0);
    if (d->s->size == 1) return NULL;
    else
    {
      PCListBuffer * snew = popBuffer(d->s, x);
      return consList(NULL, NULL, NULL, NULL, snew, 1);
    }
  }
  PCListBuffer * pnew = popBuffer(d->p, x);
  return consList(pnew, d->l, d->m, d->r, d->s, 0);
}

/*---------------------------------------------------------------------------*/

/**
 * Eject that doesn't check or change size properties of list(deque) under ejection.
 */
static PCList * simpleEject (PCList * d, PCListElem ** x)
{
  if (!d)
  {
    *x = NULL;
    return d;
  }
  if (d->suffix_only)
  {
    if (d->s->size == 1)
    {
      *x = getBufCont(d->s, 0);
      return NULL;
    }
    else
    {
      PCListBuffer * snew = ejectBuffer(d->s, x);
      return consList(NULL, NULL, NULL, NULL, snew, 1);
    }
  }
  PCListBuffer * snew = ejectBuffer(d->s, x);
  return consList(d->p, d->l, d->m, d->r, snew, 0);
}

/*---------------------------------------------------------------------------*/

/**
 * Splits buffers according to the specifications in concatenation.
 */
static void concatSplit (PCListBuffer * b, PCListBuffer ** b1, PCListBuffer ** b2, int first)
{
  *b1 = makeBuffer();
  *b2 = makeBuffer();
  int size = b->size - 1;
  switch (size)
  {
    case 2:
      if (first) 
      {
        int cp = 0;
        for (int i = 0; i < 2; i++) setBufCont(*b1, getBufCont(b, cp++), i);
        (*b1)->size = 2;
        (*b2)->size = 0;
      }
      else 
      {
        int cp = 1;
        for (int i = 0; i < 2; i++) setBufCont(*b2, getBufCont(b, cp++), i);
        (*b1)->size = 0;
        (*b2)->size = 2;
      }
      break;
    case 3:
      if (first) 
      {
        int cp = 0;
        for (int i = 0; i < 3; i++) setBufCont(*b1, getBufCont(b, cp++), i);
        (*b1)->size = 3;
        (*b2)->size = 0;
      }
      else 
      {
        int cp = 1;
        for (int i = 0; i < 3; i++) setBufCont(*b2, getBufCont(b, cp++), i);
        (*b1)->size = 0;
        (*b2)->size = 3;
      }
      break;
    case 4:
      if (first) 
      {
        int cp = 0;
        for (int i = 0; i < 2; i++) setBufCont(*b1, getBufCont(b, cp++), i);
        for (int i = 0; i < 2; i++) setBufCont(*b2, getBufCont(b, cp++), i);
        (*b1)->size = 2;
        (*b2)->size = 2;
      }
      else 
      {
        int cp = 1;
        for (int i = 0; i < 2; i++) setBufCont(*b1, getBufCont(b, cp++), i);
        for (int i = 0; i < 2; i++) setBufCont(*b2, getBufCont(b, cp++), i);
        (*b1)->size = 2;
        (*b2)->size = 2;
      }
      break;
    case 5:
      if (first) 
      {
        int cp = 0;
        for (int i = 0; i < 3; i++) setBufCont(*b1, getBufCont(b, cp++), i);
        for (int i = 0; i < 2; i++) setBufCont(*b2, getBufCont(b, cp++), i);
        (*b1)->size = 3;
        (*b2)->size = 2;
      }
      else 
      {
        int cp = 1;
        for (int i = 0; i < 2; i++) setBufCont(*b1, getBufCont(b, cp++), i);
        for (int i = 0; i < 3; i++) setBufCont(*b2, getBufCont(b, cp++), i);
        (*b1)->size = 2;
        (*b2)->size = 3;
      }
      break;
    default:
      break;
  }
}

/*---------------------------------------------------------------------------*/
