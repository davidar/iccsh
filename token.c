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

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include <util/string.h>

#include "token.h"

#define MAX_TOKEN_LENGTH 32

static const char *declaration_specifiers[] = {
    /* storage class   */ "auto", "register", "static", "extern", "typedef",
    /* type specifiers */ "void", "char", "short", "int", "long",
                          "float", "double", "signed", "unsigned",
    /* type qualifiers */ "const", "volatile",
    NULL
};

char *get_token(const char **strp) {
    const char *s = *strp;
    char token[MAX_TOKEN_LENGTH];
    while(isspace(*s))
        s++;
    if(!*s)
        return NULL;
    if(!isalnum(*s) && *s != '_') {
        token[0] = *s++;
        token[1] = 0;
    } else {
        char *t = token;
        while((t < token + MAX_TOKEN_LENGTH - 1) &&
                (isalnum(*s) || *s == '_'))
            *t++ = *s++;
        *t = 0;
    }
    *strp = s;
    return strdup(token);
}

int is_identifier(const char *s) {
    if(!isalpha(*s) && *s != '_')
        return 0;
    for(s++; *s; s++)
        if(!isalnum(*s) && *s != '_')
            return 0;
    return 1;
}

int variable_declaration(const char *line, char **name) {
    int specifiers = 0;
    char *token = NULL;
    while((token = get_token(&line))) {
        if(strcmp(token, "*") == 0) {
            free(token);
            continue;
        } else if(contains_string(declaration_specifiers, token)) {
            free(token);
            specifiers++;
            continue;
        } else if(string_in(token, "struct", "union", "enum", NULL)) {
            free(token);
            free(get_token(&line));
            specifiers++;
            continue;
        } else if(is_identifier(token)) {
            if(!specifiers)
                break;
            char *next_token = get_token(&line);
            if(next_token == NULL) {
                /* first identifier, last token */
                *name = token;
                return 1;
            } else if(strcmp(next_token, "=") == 0) {
                free(next_token);
                *name = token;
                return 1;
            } else {
                free(next_token);
                break;
            }
        } else { /* unrecognised */
            break;
        }
    }
    if(token) free(token);
    return 0;
}
