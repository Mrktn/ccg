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

static const char *type2varid[_typemax] = {"c", "uc", "s", "us", "i", "ui"};
const char *type2str[_typemax] = {"int8_t", "uint8_t", "int16_t", "uint16_t", "int32_t", "uint32_t"};

void addVariableToList(Variable *variable, VariableList **list)
{
    if(!*list)
    {
        *list = xmalloc(sizeof(*list));
        (*list)->variable = variable;
        (*list)->next = NULL;
    }

    else
    {
        VariableList *v;

        for(v = *list; v->next; v = v->next);
        v->next = xmalloc(sizeof(v->next));
        v->next->variable = variable;
        v->next->next = NULL;
    }
}

static char *makeVariableName(VariableType type, unsigned char qualifiers, VariableList *list)
{
    VariableList *v;
    size_t cnt = 0, sz = 2;
    char *name, buf[8];

    if(list)
        foreach_variable(v, list)
            if(v->variable->type == type)
                ++cnt;

    if(type == _u8 || type == _u16 || type == _u32)
        ++sz;

    sz += strlen(type2varid[type]);

    sprintf(buf, "%lu", cnt);
    sz += strlen(buf);
    name = xcalloc(sz, 1);

    strcat(name, type2varid[type]);
    strcat(name, buf);

    return name;
}

Variable *makeRandomVariable(VariableList *scope)
{
    Variable *ret = xmalloc(sizeof(Variable));

    ret->type = rand() % _typemax;
    ret->permissions = P_WRITEABLE;
    ret->initializer = makeRandomIntegerConstant(VARTYPE_TO_BITNESS(ret->type));

    ret->name = makeVariableName(ret->type, 0, scope);
    return ret;
}

void printVariable(Variable *variable)
{
    printf("%s %s = %s;\n", type2str[variable->type], variable->name, variable->initializer.value);
}

void copyVariableList(VariableList *src, VariableList **dest)
{
    VariableList *v;
    
    foreach_variable(v, src)
    {
        Variable *var = xmalloc(sizeof(*var));
        
        /* OK. We make a copy of the variable, but no need to duplicate its name */
        memcpy(var, v->variable, sizeof(*var));
        
        /* Then we add it to the destination list. So much allocated memory without a single free... FIXME. */
        addVariableToList(var, dest);
    }
}

size_t numVariablesInList(VariableList *list)
{
    VariableList *v;
    size_t i = 0;

    foreach_variable(v, list)
        ++i;

    return i;
}

/* Pick a variable from the `list' */
Variable *pickRandomVariable(VariableList *list)
{
    VariableList *v;
    size_t n = numVariablesInList(list), i = 0, t = rand() % n + 1;

    foreach_variable(v, list)
        if(++i == t)
            return v->variable;

    die("unreachable, no variables in scope !");
    return NULL;
}

bool writeableVariablesExist(VariableList *list)
{
    VariableList *v;

    foreach_variable(v, list)
        if(v->variable->permissions & P_WRITEABLE)
            return true;

    return false;
}

Variable *pickRandomWriteableVariable(VariableList *list)
{
    Variable *var = NULL;
    VariableList *v;
    size_t n = numVariablesInList(list);

    do
    {
        size_t t = rand() % n + 1, i = 0;

        foreach_variable(v, list)
        {
            if(++i == t)
            {
                var = v->variable;
                break;
            }
        }
    } while(!(var->permissions & P_WRITEABLE));

    /* We are sure not to go in an infinite loop since we have already checked for them using writeableVariablesExist */

    return var;
}

void makeGlobalVariables(void)
{
    size_t i = 0;

    program.numglobalvars = rand() % 10 + 1;

    for(; i < program.numglobalvars; ++i)
    {
        Variable *v = makeRandomVariable(program.globalvars);
        addVariableToList(v, &program.globalvars);
    }
}
