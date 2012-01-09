#include <cmath>
#include <iostream>
#include <sstream>
#include <cassert>

#define SC_USE_PNG

#include "runtime/scout_gpu.h"
#include "runtime/cuda/cuda.h"
#include "runtime/init_mac.h"
#include "runtime/opengl/glSDL.h"
#include "runtime/tbq.h"

using namespace std;
using namespace scout;

tbq_rt* __sc_tbq;
glSDL* __sc_glsdl = 0;

size_t __sc_initial_width = 768;
size_t __sc_initial_height = 768;

extern "C"
void __sc_queue_block(void* blockLiteral, int numDimensions, int numFields){
  __sc_tbq->run(blockLiteral, numDimensions, numFields);
}

void __sc_init_sdl(size_t width, size_t height){

  if (__sc_glsdl) {
    __sc_glsdl->resize(width, height);
  } else {
    __sc_glsdl = new glSDL(width, height);
  }
}

void __sc_init(int argc, char** argv, bool gpu){
  __sc_tbq = new tbq_rt;

  if(gpu){
    __sc_init_sdl(__sc_initial_width, __sc_initial_height);
    __sc_init_cuda();
  }
}

void __sc_init(bool gpu){
  __sc_init(0, 0, gpu);
}

void __sc_end(){

}

double cshift(double a, int dx, int axis){
  return 0.0;
}

float cshift(float a, int dx, int axis){
  return 0.0;
}

int cshift(int a, int dx, int axis){
  return 0.0;
}
