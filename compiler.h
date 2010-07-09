#ifndef __COMPILER_H__
#define __COMPILER_H__

#include <libtcc.h>

/**
 * Initialize the C compiler.
 */
void compiler_init(void);

/**
 * Compile C source code.
 */
TCCState *compile(const char *global, const char *local);

/**
 * Relocate the given TCC state, and return the allocated memory.
 */
void *relocate(TCCState *tccs);

/**
 * Relocate the given TCC state and run the main function.
 */
void run(TCCState *tccs);

/**
 * Include the given header in subsequent compilations.
 */
void add_include(const char *file);

/**
 * Link against the given library in subsequent compilations.
 */
void add_library(const char *name);

#endif
