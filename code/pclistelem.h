/*-------------------------------------------------------------*
 * pclistelem.h - header for elements used in PCList           *
 *-------------------------------------------------------------*
 * This file is a part of a Bachelor's thesis named Persistent *
 * data structures and their applications.                     *
 *                                                             *
 * Author: Josef Malík                                         *
 *-------------------------------------------------------------*
 */

#ifndef __PCLISTELEM_H__
#define __PCLISTELEM_H__

#include "common.h"

struct PCListElem_struct
{
  PCListBuffer * fmb;
  PCList       * rl;
  PCListBuffer * lmb;
  VAL_TYPE       val;
  int            is_triple;
};

PCListElem * makeElem   (VAL_TYPE content);
PCListElem * makeTriple (PCListBuffer * fmb, PCList * rl, PCListBuffer * lmb);
void         printElem  (PCListElem * x);

#endif /*__PCLISTELEM_H__*/
