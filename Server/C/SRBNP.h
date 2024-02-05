#include "SRBNP/_Imports.h"

#if __STDC_VERSION__ < 201112L || __STD_NO_ATOMICS__
#error "Atomics Not Supported!"
#else
#define HAS_ATOMICS
#endif
