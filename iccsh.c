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
#include "token.h"

#define VERSION "0.1dev"

static int input_cb(char *line) {
    if(line[strlen(line)-1] == '\\') {
        line[strlen(line)-1] = 0;
        return 1;
    }
    return 0;
}

int main(int argc, char **argv) {
    compiler_init();
    symbols_init();
    printf("iCCsh version %s\n", VERSION);
    printf("type 'help' for usage information\n");
    while(1) {
        char *inp = input(">>> ", "... ", input_cb);
        if(inp == NULL) {
            printf("\n");
            printf(":quit\n");
            command("quit")(NULL);
        }
        char *line = inp; trim(&line);
        char *name = NULL;
        if(!*line) {
            continue;
        } else if(*line == ':') {
            char *command_name = line + 1;
            char *args = split(line);
            command(command_name)(args);
        } else if(*line == '%') {
            printf(":printf %s\n", line);
            command("printf")(line);
        } else if(strcmp(line, "help") == 0 || strcmp(line, "h") == 0) {
            printf(":help\n");
            command("help")(NULL);
        } else if(is_identifier(line)) {
            printf(":info %s\n", line);
            command("info")(line);
        } else if(variable_declaration(line, &name)) {
            char *args; asprintf(&args, "%s %s", name, line);
            printf(":declare %s\n", args);
            command("declare")(args);
            free(args);
        } else {
            printf(":eval %s\n", line);
            command("eval")(line);
        }
        if(name) free(name);
        free(inp);
    }
    return 0;
}
