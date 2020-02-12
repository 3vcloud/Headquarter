#ifndef _CRT_SECURE_NO_WARNINGS
# define _CRT_SECURE_NO_WARNINGS
#endif

#ifndef _CRT_NONSTDC_NO_DEPRECATE
# define _CRT_NONSTDC_NO_DEPRECATE
#endif

#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// #include "time.h"
// #include "assert.h"
// #include "thread.h"
// #include "unixstd.h"

#include "macro.h"
#include "win32/win32.h"

#include "win32/time.c"
#include "win32/dlfunc.c"
#include "win32/thread.c"
#include "win32/process.c"

#include "log.c"
#include "array.c"
#include "timer.c"
#include "rbtree.c"
