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

#define _GNU_SOURCE
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <util/debug.h>
#include <util/io.h>
#include <util/list.h>
#include <util/string.h>

#include "compiler.h"
#include "memory.h"
#include "symbols.h"

#define SOURCE_TEMPLATE "%s\n;\nvoid main(void) {\n%s\n;\n}\n"

static struct list *directives;
static struct list *libs;

void compiler_init(void) {
    directives = create_list();
    libs = create_list();
}

TCCState *compile(const char *global, const char *local) {
    TCCState *tccs = tcc_new();
    if(!tccs)
        return NULL;
    tcc_set_output_type(tccs, TCC_OUTPUT_MEMORY);
    provide_symbols(tccs);

    struct list_node *n;
    for(n = list_first(libs); n; n = list_node_next(n)) {
        const char *name = list_node_value(n);
        DEBUG_PRINTF("Adding lib%s\n", name);
        tcc_add_library(tccs, name);
    }

    char *buf = strdup("");
    for(n = list_first(directives); n; n = list_node_next(n)) {
        const char *directive = list_node_value(n);
        astrcat(&buf, directive);
    }
    astrcatf(&buf, symbol_declarations());
    asprintfa(&buf, SOURCE_TEMPLATE, global, local);

    DEBUG_PRINTF("Compiling...\n%s\n", buf);
    if(tcc_compile_string(tccs, buf) == -1) {
        tcc_delete(tccs);
        tccs = NULL;
    }
    free(buf);
    return tccs;
}

void *relocate(TCCState *tccs) {
    int size = tcc_relocate(tccs, NULL);
    if(size == -1)
        return NULL;
    void *mem = memory_alloc(size);
    tcc_relocate(tccs, mem);
    DEBUG_PRINTF("Relocated to %p..%p (%d bytes)\n",
            mem, mem + size - 1, size);
    return mem;
}

void run(TCCState *tccs) {
    if(relocate(tccs)) {
        DEBUG_PRINTF("Running...\n");
        void (*main)(void) = tcc_get_symbol(tccs, "main");
        if(main) {
            fflush(stdout);
            pid_t pid = fork();
            if(pid == 0) {
                main();
                exit(0);
            }
            int status;
            waitpid(pid, &status, 0);
            fflush(stdout);
            if(WIFEXITED(status)) {
                int exit_status = WEXITSTATUS(status);
                if(exit_status)
                    fprintf(stderr, "Exited with status %d\n", exit_status);
            } else if(WIFSIGNALED(status)) {
                psignal(WTERMSIG(status), NULL);
            }
        }
    }
    tcc_delete(tccs);
}

void add_directive(const char *directive) {
    char *line; asprintf(&line, "%s\n", directive);
    list_append(directives, (void *) line);
}

void add_library(const char *lib) {
    list_append(libs, (void *) strdup(lib));
}

void compiler_close(void) {
    list_destroy(directives, 1);
    list_destroy(libs, 1);
}
