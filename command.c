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

#include <linenoise.h>

#include <util/io.h>
#include <util/string.h>

#include "command.h"
#include "compiler.h"
#include "memory.h"
#include "symbols.h"

static char *input_block(void) {
    char *line = strdup("");
    char *s;
    while((s = linenoise("... "))) {
        if(!*s)
            return line;
        linenoiseHistoryAdd(s);
        astrcatf(&line, s);
        astrcat(&line, "\n");
    }
    free(line);
    return NULL;
}

static void define_symbol(char *args, char delim) {
    const char *name = args;
    if(!name) return;
    char *decl = split(args);
    if(!decl) decl = input_block();
    if(!decl) return;
    char *defn = strdup(decl);
    strchrr(decl, delim, 0);
    trim(&decl);
    if(!*decl) return;

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
    define_symbol(args, '=');
}

static void command_define(char *args) {
    if(!args) args = input_block();
    if(!args) return;
    char *directive = strdup("#define ");
    char *line = args;
    char *eol;
    while((eol = strchrr(line, '\n', 0))) {
        asprintfa(&directive, "%s \\\n", line);
        line = eol+1;
    }
    astrcat(&directive, line);
    add_directive(directive); free(directive);
}

static void command_delete(char *name) {
    if(!name) return;
    delete_symbol(name);
}

static void command_eval(char *stmt) {
    if(!stmt) stmt = input_block();
    if(!stmt) return;
    TCCState *tccs = compile("", stmt);
    if(tccs) run(tccs);
}

static void command_function(char *args) {
    define_symbol(args, '{');
}

static void command_help(char *args) {
    FILE *fp = fopen("help.txt", "r");
    char *line;
    while((line = read_line(fp))) {
        printf("%s\n", line);
        free(line);
    }
    fclose(fp);
    fflush(stdout);
}

static void command_include(char *file) {
    if(!file) return;
    char *directive;
    if((starts_with(file, "<") && ends_with(file, ">")) ||
            (starts_with(file, "\"") && ends_with(file, "\"")))
        asprintf(&directive, "#include %s", file);
    else
        asprintf(&directive, "#include <%s.h>", file);
    add_directive(directive); free(directive);
}

static void command_info(char *name) {
    if(!name) return;
    const char *decl = symbol_declaration(name);
    if(decl) {
        printf("%s\n", decl);
        fflush(stdout);
    } else {
        fprintf(stderr, "'%s' has not been declared\n", name);
    }
}

static void command_library(char *lib) {
    if(!lib) return;
    add_tcc_opt(tcc_add_library, lib);
}

static void command_printf(char *args) {
    const char *format = args;
    if(!format) return;
    const char *arg = split(args);
    if(!arg) return;
    char *stmt; asprintf(&stmt, "printf(\"%s\\n\", %s)", format, arg);
    command_eval(stmt);
    free(stmt);
}

static void command_quit(char *args) {
    linenoiseHistorySave("history.txt");
    symbols_close();
    compiler_close();
    memory_close();
    exit(0);
}

static void command_undefine(char *macro) {
    if(!macro) return;
    char *directive; asprintf(&directive, "#undef %s", macro);
    add_directive(directive); free(directive);
}

static void command_not_found(char *args) {
    fprintf(stderr, "Command not found\n");
}

struct command {
    const char *name;
    void (*fn)(char *);
};

static struct command commands[] = {
    { "declare",  command_declare  },
    { "decl",     command_declare  },
    { "d",        command_declare  },

    { "define",   command_define   },
    { "def",      command_define   },

    { "delete",   command_delete   },
    { "del",      command_delete   },

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

    { "undefine", command_undefine },
    { "undef",    command_undefine },

    { NULL,       command_not_found}
};

void (*command(const char *name))(char *) {
    struct command *c;
    for(c = commands; c->name; c++)
        if(strcmp(name, c->name) == 0)
            break;
    return c->fn;
}
