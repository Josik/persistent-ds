/*--------------------------------------------------------------*
 * pclist.h - header for confluently persistent catenable lists *
 *--------------------------------------------------------------*
 * This file is a part of a Bachelor's thesis named Persistent  *
 * data structures and their applications.                      *
 *                                                              *
 * Author: Josef Malík                                          *
 *--------------------------------------------------------------*
 */

#ifndef __PCL_H__
#define __PCL_H__

#include "common.h"
#include "pclistbuffer.h"

struct PCList_struct
{
  PCListBuffer * p;
  PCList       * l;
  PCListBuffer * m;
  PCList       * r;
  PCListBuffer * s;
  int            suffix_only;
};

PCList * makeList  (VAL_TYPE val);
PCList * copyList  (PCList * d);
PCList * consList  (PCListBuffer * p, PCList * l, PCListBuffer * m, PCList * r, PCListBuffer * s, int so); 
PCList * push      (PCList * d, VAL_TYPE val);
PCList * pop       (PCList * d, VAL_TYPE * val);
PCList * inject    (PCList * d, VAL_TYPE val);
PCList * eject     (PCList * d, VAL_TYPE * val);
PCList * catenate  (PCList * d1, PCList * d2);
void     gPrefix   (PCList * d);
void     gSuffix   (PCList * d);
int      isTuple   (PCList * d);

#endif /*__PCL_H__*/
