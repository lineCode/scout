/*
 * -----  Scout Programming Language -----
 *
 * This file is distributed under an open source license by Los Alamos
 * National Security, LCC.  See the file License.txt (located in the
 * top level of the source distribution) for details.
 * 
 *-----
 * 
 */

#ifndef SCOUT_TBQ_H_
#define SCOUT_TBQ_H_

#include <list>

#include "runtime/types.h"

namespace scout 
{

  void performIteration(void (^block)(void*,int*,int*,int*,...),
			int xStart, int xEnd,
			int yStart=-1, int yEnd=-1,
			int zStart=-1, int zEnd=-1);
}

#endif