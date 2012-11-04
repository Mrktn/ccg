/** C Code Generator
 *
 * Copyright (C) 2012, Antoine Balestrat <antoine.balestrat@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom
 * the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include "ccg.h"

/* `nesting' is the nesting level of the block. func(){0 {1} {1 {2}}} */
Block *makeBlock(VariableList *pscope, unsigned nesting)
{
    size_t i;
    Block *ret = xmalloc(sizeof(Block));
    VariableList *scope = NULL;

    /* Make a copy of the list of the currently usable variables.
    This way, we will be able to add variables in a clean list without corrupting the one that's been passed by parameters */
    copyVariableList(pscope, &scope);

    ret->localvars = NULL, ret->statementlist = NULL;
    ret->numlocalvars = ret->numstatements = 0;

    /* 1st step: generate some random vars (at least 1 if we are in the body of a function */
    ret->numlocalvars = rand() % cmdline.max_localvars + !nesting;

    for(i = 0; i < ret->numlocalvars; ++i)
    {
        Variable *tmp = makeVariable(scope, _randomvartype);
        addVariableToList(tmp, &(ret->localvars));
        addVariableToList(tmp, &scope);
    }

    /* 2nd step: generate some random statements ! */
    if(nesting < cmdline.max_block_nesting)
    {
        ret->numstatements = rand() % cmdline.max_statements_per_block;

        for(i = 0; i < ret->numstatements; ++i)
        {
            Statement *tmp = makeStatement(scope, nesting);
            addStatementToList(tmp, &ret->statementlist);
        }
    }

    return ret;
}


void printBlock(Block *block)
{
    VariableList *v;
    StatementList *s;

    puts("{");

    foreach(v, block->localvars)
        printVariableDecl(v->variable);

    foreach(s, block->statementlist)
        printStatement(s->statement);

    puts("}");
}
