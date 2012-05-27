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

char *makeRandomFunctionName(void)
{
    char *name;
    char buff[8];

    sprintf(buff, "%d", program.numfunctions);
    name = malloc(strlen(buff) + 1);
    sprintf(name, "func_%d", program.numfunctions);

    return name;
}

/* At this point, only global variables are available */
Function *makeRandomFunction(bool params)
{
    short i;
    Function *ret = xmalloc(sizeof(Function));
    VariableList *scope = params ? NULL : program.globalvars;

    ret->paramlist = NULL;
    ret->returntype = rand() % _typemax;
    ret->name = makeRandomFunctionName();
    ret->numparams = params ? rand() % (cmdline.max_function_parameters + 1) : 0;

    program.numfunctions++;

    if(params)
    {
        copyVariableList(program.globalvars, &scope);

        for(i = 0; i < ret->numparams; ++i)
        {
            Variable *v = makeRandomVariable(scope);
            addVariableToList(v, &ret->paramlist);
            addVariableToList(v, &scope);
        }
    }

    ret->body = makeRandomBlock(scope, 0);

    addFunctionToList(ret, &program.functions);
    return ret;
}

void printFunctionPrototype(Function *function)
{
    VariableList *v;

    printf("%s %s(", type2str[function->returntype], function->name);

    foreach_variable(v, function->paramlist)
    {
        printf(v == function->paramlist ? "" : ", ");
        printf("%s %s", type2str[v->variable->type], v->variable->name);
    }

    printf(")");
}

void printFunction(Function *function)
{
    printFunctionPrototype(function);
    putchar('\n');
    printBlock(function->body);
}