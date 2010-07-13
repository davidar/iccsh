/*
* Copyright (c) 2010 David A Roberts <d@vidr.cc>
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
*/

#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#include <util/debug.h>

#include "memory.h"

#ifndef PROT_ALL
#define PROT_ALL (PROT_EXEC|PROT_READ|PROT_WRITE)
#endif

#define MEM_SIZE (1<<20) /* 1 MiB */
#define MEM_PATH_TEMPLATE "/tmp/iccsh.mmap.XXXXXX"

static char *filename;
static int fd;
static void *map;
static void *mem;

void memory_init(void) {
    filename = strdup(MEM_PATH_TEMPLATE); fd = mkstemp(filename);
    DEBUG_PRINTF("Created temporary file: %s\n", filename);
    lseek(fd, MEM_SIZE-1, SEEK_SET); write(fd, "", 1);
    mem = map = mmap(NULL, MEM_SIZE, PROT_ALL, MAP_SHARED, fd, 0);
}

void *memory_alloc(size_t size) {
    if(mem + size > map + MEM_SIZE)
        return NULL;
    void *addr = mem;
    mem += size;
    DEBUG_PRINTF("Allocated %d bytes\n", size);
    return addr;
}

void memory_close(void) {
    munmap(map, MEM_SIZE);
    close(fd);
    unlink(filename);
}
