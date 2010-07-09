#ifndef __SYMBOLS_H__
#define __SYMBOLS_H__

#include <libtcc.h>

/**
 * Initialize the symbol table.
 */
void symbols_init(void);

/**
 * Add the given symbol to the table.
 */
void add_symbol(const char *name, void *addr, const char *decl);

/**
 * Add all symbols currenlty in the table to the given TCC state.
 */
void provide_symbols(TCCState *tccs);

/**
 * Return a string of external symbol declarations suitable for inclusion in
 * a C source file.
 */
char *symbol_declarations(void);

/**
 * Return the declaration for the symbol with the given name.
 */
const char *symbol_declaration(const char *name);

#endif
