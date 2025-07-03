#ifndef UTILS_H
#define UTILS_H
#include <time.h>
typedef unsigned long ulong;
inline ulong millis(){
    return static_cast<ulong>(clock()/ (0.001*CLOCKS_PER_SEC));
}
#endif // UTILS_H
