/*-------------------------------------------------------------*
 * pclistbuffer.h - header for buffers used in PCList          *
 *-------------------------------------------------------------*
 * This file is a part of a Bachelor's thesis named Persistent *
 * data structures and their applications.                     *
 *                                                             *
 * Author: Josef Malík                                         *
 *-------------------------------------------------------------*
 */

#ifndef __PCLISTBUFFER_H__
#define __PCLISTBUFFER_H__

#include "common.h"
#include "pclistelem.h"

struct PCListBuffer_struct
{
  PCListElem * buf[10];
  int          size;
};

PCListBuffer * makeBuffer   (void);
PCListBuffer * pushBuffer   (PCListBuffer * a, PCListElem * x);
PCListBuffer * popBuffer    (PCListBuffer * a, PCListElem ** x);
PCListBuffer * injectBuffer (PCListBuffer * a, PCListElem * x);
PCListBuffer * ejectBuffer  (PCListBuffer * a, PCListElem ** x);
void           setBufCont   (PCListBuffer * a, PCListElem * x, int pos);
PCListElem   * getBufCont   (PCListBuffer * a, int pos);

#endif /*__PCLISTBUFFER_H__*/
