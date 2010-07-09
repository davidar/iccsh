#ifndef __TOKEN_H__
#define __TOKEN_H__

/**
 * Return the next token in the given string.
 */
char *get_token(const char **strp);

/**
 * Is the given token a C identifier?
 */
int is_identifier(const char *s);

/**
 * Does the given line represent a variable declaration?
 * If so, assign the name of the variable to the second argument.
 */
int variable_declaration(const char *line, char **name);

#endif
