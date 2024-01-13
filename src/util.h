#ifndef UTIL_H
#define UTIL_H

#include <stdbool.h>

#define MIN(A, B) ((A) < (B) ? (A) : (B))
#define MAX(A, B) ((A) > (B) ? (A) : (B))

bool WithinHalf(float f1, float f2);

#endif // UTIL_H
