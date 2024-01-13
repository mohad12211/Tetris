#include <stdbool.h>

#include "util.h"

bool WithinHalf(float f1, float f2) { return f1 >= f2 && f1 - 0.5 <= f2; }
