/* C Code Generator
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

#define VARIABLE_IS_NOT_POINTABLE(var) (pointerDepth(var) >= cmdline.max_pointer_depth)

static char *makePointerName(VariableList *scope)
{
    char *ret, buff[16];

    sprintf(buff, "ptr_%zu", numVariablesInScope(scope));
    ret = xmalloc(strlen(buff) + 1);
    strcpy(ret, buff);

    return ret;
}

void makePointer(Variable *var, VariableList *scope)
{
    var->pointer.pointed = pickPointableVariable(scope);
    var->name = makePointerName(scope);
}

Variable *pickPointableVariable(VariableList *scope)
{
    VariableList *v;
    Variable *ret = NULL;
    size_t n = numVariablesInScope(scope);

    do
    {
        size_t t = rand() % n, i = 0;

        foreach(v, scope)
        {
            if(i++ == t)
                ret = v->variable;
        }

    } while(VARIABLE_IS_NOT_POINTABLE(ret));

    return ret;
}

inline size_t pointerDepth(Variable *var)
{
    size_t n = 0;

    while(var->type == _pointer)
    {
        ++n;
        var = var->pointer.pointed;
    }

    return n;
}

inline bool pointersInScope(VariableList *scope)
{
    VariableList *v;

    foreach(v, scope)
    {
        if(v->variable->type == _pointer)
            return true;
    }

    return false;
}

size_t numPointersInScope(VariableList *scope)
{
    size_t i = 0;
    VariableList *v;

    foreach(v, scope)
    {
        if(v->variable->type == _pointer)
            ++i;
    }

    return i;
}

/* This function goes through all the pointers to find the type of the ultimately pointed integer ! */
IntegerType ultimateType(Variable *var)
{
    while(var->type == _pointer)
        var = var->pointer.pointed;

    return var->intvar.type;
}

char *maxDerefdPointer(Variable *var)
{
    static char buffer[32];
    size_t depth = pointerDepth(var);

    memset(buffer, '*', depth);
    buffer[depth] = 0;

    strcat(buffer, var->name);

    return buffer;
}

void printPointerDecl(Variable *var)
{
    size_t i, depth = pointerDepth(var);

    printf("%s ", inttype2str[ultimateType(var)]);

    for(i = 0; i < depth; ++i)
        putchar('*');

    printf("%s = &%s;\n", var->name, var->pointer.pointed->name);
}
