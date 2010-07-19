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

#include <hashtable.h>
#include <hashtable_itr.h>

#include <util/hash.h>
#include <util/io.h>
#include <util/string.h>

#include "symbols.h"

#define INIT_TABLE_SIZE 16

static struct hashtable *symbol_table;

struct symbol {
    const char *name;
    void *addr;
    const char *decl;
};

DEFINE_HASHTABLE_INSERT(insert_symbol, const char, struct symbol);
DEFINE_HASHTABLE_SEARCH(search_symbol, const char, struct symbol);
DEFINE_HASHTABLE_REMOVE(remove_symbol, const char, struct symbol);

void symbols_init(void) {
    symbol_table = create_hashtable(INIT_TABLE_SIZE,
            (unsigned int (*)(void *)) hash_djb2,
            (int (*)(void *, void *)) streq);
}

void add_symbol(const char *name, void *addr, const char *decl) {
    if(!addr) return;
    struct symbol *s;
    if((s = remove_symbol(symbol_table, name))) {
        fprintf(stderr, "Warning: '%s' redeclared as '%s'\n", s->decl, decl);
        free((void *) s->decl);
        free(s);
    }
    s = malloc(sizeof(struct symbol));
    s->name = strdup(name);
    s->addr = addr;
    s->decl = strdup(decl);
    insert_symbol(symbol_table, s->name, s);
}

void delete_symbol(const char *name) {
    struct symbol *s;
    if((s = remove_symbol(symbol_table, name))) {
        free((void *) s->decl);
        free(s);
    }
}

void provide_symbols(TCCState *tccs) {
    if(hashtable_count(symbol_table)) {
        struct hashtable_itr *itr = hashtable_iterator(symbol_table);
        do {
            struct symbol *s = hashtable_iterator_value(itr);
            tcc_add_symbol(tccs, s->name, s->addr);
        } while(hashtable_iterator_advance(itr));
        free(itr);
    }
}

char *symbol_declarations(void) {
    char *decls = strdup("");
    if(hashtable_count(symbol_table)) {
        struct hashtable_itr *itr = hashtable_iterator(symbol_table);
        do {
            struct symbol *s = hashtable_iterator_value(itr);
            asprintfa(&decls, "extern %s;\n", s->decl);
        } while(hashtable_iterator_advance(itr));
        free(itr);
    }
    return decls;
}

const char *symbol_declaration(const char *name) {
    struct symbol *symbol = search_symbol(symbol_table, name);
    if(!symbol)
        return NULL;
    return symbol->decl;
}

void symbols_close(void) {
    if(hashtable_count(symbol_table)) {
        struct hashtable_itr *itr = hashtable_iterator(symbol_table);
        do {
            struct symbol *s = hashtable_iterator_value(itr);
            free((void *) s->decl);
            free(s);
        } while(hashtable_iterator_remove(itr));
        free(itr);
    }
    hashtable_destroy(symbol_table, 0);
}
