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
        char *command_name = NULL;
        char *args = line;
        char *name = NULL;
        int echo_command = 1;
        int free_args = 0;
        if(!*line) {
            continue;
        } else if(*line == ':') {
            command_name = line + 1;
            args = split(line);
            echo_command = 0;
        } else if(*line == '%') {
            command_name = "printf";
        } else if(strcmp(line, "help") == 0) {
            command_name = "help";
            args = "";
        } else if(is_identifier(line)) {
            command_name = "info";
        } else if(variable_declaration(line, &name)) {
            command_name = "declare";
            asprintf(&args, "%s %s", name, line);
            free_args = 1;
        } else {
            command_name = "eval";
        }
        if(echo_command) {
            char *args_short = strdup(args);
            char *nl = strchr(args_short, '\n');
            if(nl) strcpy(nl, "...");
            printf(":%s %s\n", command_name, args_short);
            free(args_short);
        }
        command(command_name)(args);
        if(free_args) free(args);
        if(name) free(name);
        free(inp);
    }
    return 0;
}
