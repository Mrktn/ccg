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

static char const * const inttype2varid[_inttypemax] = {"c", "uc", "s", "us", "i", "ui"};
char const * const inttype2str[_inttypemax] = {"int8_t", "uint8_t", "int16_t", "uint16_t", "int32_t", "uint32_t"};

static char *makeIntegerName(IntegerType type, VariableList *scope)
{
    char buffer[16] = {0}, *ret;

    sprintf(buffer, "%s_%zu", inttype2varid[type], numVariablesInScope(scope));
    ret = xmalloc(strlen(buffer) + 1);
    strcpy(ret, buffer);

    return ret;
}

void makeInteger(Variable *var, VariableList *scope)
{
    var->intvar.type = rand() % _inttypemax;
    var->intvar.initializer = makeIntegerConstant(INTEGERTYPE_SIZE(var->intvar.type));
    var->name = makeIntegerName(var->intvar.type, scope);
}

size_t numIntegersInScope(VariableList *scope)
{
    size_t i = 0;
    VariableList *v;

    foreach(v, scope)
    {
        if(v->variable->type == _integer)
            ++i;
    }

    return i;
}

void printIntegerDecl(Variable *var)
{
    printf("%s %s = %s;\n", inttype2str[var->intvar.type], var->name, var->intvar.initializer.value);
}
