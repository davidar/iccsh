#ifndef __COMMAND_H__
#define __COMMAND_H__

/**
 * Return the function for the command with the given name.
 */
void (*command(const char *name))(char *);

#endif
