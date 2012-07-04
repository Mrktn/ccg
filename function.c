/* C Code Generator
 *
 * Copyright (C) 2012, Antoine Balestrat <merkil@savhon.org>
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

void addFunctionToList(Function *function, FunctionList **list)
{
    if(!*list)
    {
        *list = xmalloc(sizeof(*list));
        (*list)->function = function;
        (*list)->next = NULL;
    }

    else
    {
        FunctionList *v;

        for(v = *list; v->next; v = v->next);
        v->next = xmalloc(sizeof(v->next));
        v->next->function = function;
        v->next->next = NULL;
    }
}

char *makeFunctionName(void)
{
    char *name;
    char buff[8];

    sprintf(buff, "%zu", program.numfunctions);
    name = malloc(strlen(buff) + 1);
    sprintf(name, "func_%zu", program.numfunctions);

    return name;
}

/* At this point, only global variables are available */
Function *makeFunction(bool params)
{
    short i;
    Function *ret = xmalloc(sizeof(Function));
    VariableList *scope = params ? NULL : program.globalvars;

    ret->paramlist = NULL;
    ret->returntype = rand() % _inttypemax;
    ret->name = makeFunctionName();
    ret->numparams = params ? rand() % (cmdline.max_function_parameters + 1) : 0;

    program.numfunctions++;

    if(params)
    {
        copyVariableList(program.globalvars, &scope);

        for(i = 0; i < ret->numparams; ++i)
        {
            Variable *v = makeVariable(scope, _integer);
            addVariableToList(v, &ret->paramlist);
            addVariableToList(v, &scope);
        }
    }

    ret->body = makeBlock(scope, 0);

    addFunctionToList(ret, &program.functions);
    return ret;
}

void printFunctionPrototype(Function *function)
{
    VariableList *v;

    printf("%s %s(", inttype2str[function->returntype], function->name);

    foreach(v, function->paramlist)
    {
        printf(v == function->paramlist ? "" : ", ");
        printVariableType(v->variable);
        printf(" %s", v->variable->name);
    }

    printf(")");
}

void printFunction(Function *function)
{
    printFunctionPrototype(function);
    putchar('\n');
    printBlock(function->body);
}