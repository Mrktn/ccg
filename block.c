/*  C Code Generator
 *
 *  Copyright (c) 2012, Antoine Balestrat, merkil@savhon.org
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "ccg.h"

/* `nesting' is the nesting level of the block. func(){0 {1} {1 {2}}} */
Block *makeRandomBlock(VariableList *pscope, unsigned nesting)
{
    size_t i;
    Block *ret = xmalloc(sizeof(Block));
    VariableList *scope = NULL;

    /* Make a copy of the list of the currently usable variables. This way, we will be able to add variables in a clean list without corrupting the one that's been passed by parameters */
    /* FIXME: this is a poor solution. It wastes a lot of memory */
    copyVariableList(pscope, &scope);

    ret->localvars = NULL, ret->statementlist = NULL;
    ret->numlocalvars = ret->numstatements = 0;

    /* 1st step: generate some random vars (at least 1 if we are in the body of a function */
    ret->numlocalvars = rand() % cmdline.max_localvars + !nesting;

    for(i = 0; i < ret->numlocalvars; ++i)
    {
        Variable *tmp = makeRandomVariable(scope);
        addVariableToList(tmp, &(ret->localvars));
        addVariableToList(tmp, &scope);
    }

    /* 2nd step: generate some random statements ! */
    if(nesting < cmdline.max_block_nesting)
    {
        ret->numstatements = rand() % cmdline.max_statements_per_block;

        for(i = 0; i < ret->numstatements; ++i)
        {
            Statement *tmp = makeRandomStatement(scope, nesting);
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

    foreach_variable(v, block->localvars)
        printVariable(v->variable);

    foreach_statement(s, block->statementlist)
        printStatement(s->statement);

    puts("}");
}
