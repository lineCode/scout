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

#ifndef SCOUT_VEC_TYPES_H_
#define SCOUT_VEC_TYPES_H_

namespace scout 
{
  

  // Some basic vector types. 
  typedef float float4v __attribute__ ((vector_size (4 * sizeof(float))));
  union float4 {
    float4v     vec;
    float       components[4];
  };
  
  typedef int   int4v   __attribute__ ((vector_size (4 * sizeof(int))));
  union int4 {
    int4v      vec;
    int        components[4];
  };
  
}

#endif 



  