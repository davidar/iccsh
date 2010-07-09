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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <util/io.h>
#include <util/string.h>

#include "command.h"
#include "compiler.h"
#include "symbols.h"

#define INPUT_BLOCK() input("... ", "... ", input_cb)

static int input_cb(char *line) {
    return line[strlen(line)-1] != '\n';
}

static void define(char *args, char delim) {
    const char *name = args;
    char *decl = split(args);
    if(!decl) decl = INPUT_BLOCK();
    char *defn = strdup(decl);
    strchrr(decl, delim, 0);

    TCCState *tccs = compile(defn, ""); free(defn);
    if(!tccs)
        return;
    if(relocate(tccs)) {
        void *addr = tcc_get_symbol(tccs, name);
        add_symbol(name, addr, decl);
    }
    tcc_delete(tccs);
}

static void command_declare(char *args) {
    define(args, '=');
}

static void command_eval(char *stmt) {
    if(!stmt) stmt = INPUT_BLOCK();
    TCCState *tccs = compile("", stmt);
    if(tccs) run(tccs);
}

static void command_function(char *args) {
    define(args, '{');
}

static void command_help(char *args) {
    FILE *fp = fopen("help.txt", "r");
    char *line;
    while((line = read_line(fp))) {
        printf("%s\n", line);
        free(line);
    }
    fclose(fp);
}

static void command_include(char *file) {
    add_include(file);
}

static void command_info(char *name) {
    const char *decl = symbol_declaration(name);
    if(decl)
        printf("%s\n", decl);
    else
        printf("'%s' has not been declared\n", name);
}

static void command_library(char *name) {
    add_library(name);
}

static void command_printf(char *args) {
    const char *format = args;
    const char *arg = split(args);
    char *stmt; asprintf(&stmt, "printf(\"%s\\n\", %s)", format, arg);
    command_eval(stmt);
    free(stmt);
}

static void command_quit(char *args) {
    exit(0);
}

struct command {
    const char *name;
    void (*fn)(char *);
};

static struct command commands[] = {
    { "declare",  command_declare  },
    { "decl",     command_declare  },
    { "d",        command_declare  },

    { "eval",     command_eval     },

    { "function", command_function },
    { "func",     command_function },
    { "f",        command_function },

    { "help",     command_help     },
    { "h",        command_help     },

    { "include",  command_include  },
    { "inc",      command_include  },
    { "i",        command_include  },

    { "info",     command_info     },

    { "library",  command_library  },
    { "lib",      command_library  },
    { "l",        command_library  },

    { "printf",   command_printf   },
    { "p",        command_printf   },

    { "quit",     command_quit     },
    { "q",        command_quit     },

    { NULL,       NULL         }
};

void (*command(const char *name))(char *) {
    struct command *c;
    for(c = commands; c->name; c++)
        if(strcmp(name, c->name) == 0)
            break;
    return c->fn;
}
