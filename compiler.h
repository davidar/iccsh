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
 * Include the given preprocessor directive in subsequent compilations.
 */
void add_directive(const char *directive);

/**
 * Link against the given library in subsequent compilations.
 */
void add_library(const char *lib);

/**
 * Free the compiler data structures.
 */
void compiler_close(void);

#endif
