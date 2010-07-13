#ifndef __MEMORY_H__
#define __MEMORY_H__

/**
 * Initialize the shared memory allocator.
 */
void memory_init(void);

/**
 * Allocate a block of shared memory.
 */
void *memory_alloc(size_t size);

/**
 * Free all allocated memory.
 */
void memory_close(void);

#endif
