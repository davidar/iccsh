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
 * Relocate the given TCC state and run the main function. The second
 * parameter controls whether to execute the function in sandbox mode.
 */
void run(TCCState *tccs, int sandbox);

/**
 * Include the given preprocessor directive in subsequent compilations.
 */
void add_directive(const char *directive);

/**
 * Set the given TCC option in subsequent compilations.
 */
void add_tcc_opt(int (*fn)(TCCState *, const char *), const char *value);

/**
 * Free the compiler data structures.
 */
void compiler_close(void);

#endif
