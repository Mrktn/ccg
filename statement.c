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

/*
    - If: 20%
    - For: 20%
    - Assignment: 30%
    - Function call: 20%
    - Return: 10%
*/
static const StatementType statarray[10] = {_if, _if, _for, _for, _assignment, _assignment, _assignment, _functioncall, _functioncall, _return};

static void buildIf(Statement*, VariableList*, unsigned);
static void buildFor(Statement*, VariableList*, unsigned);
static void buildAssignment(Statement*, VariableList*, unsigned);
static void buildFunctionCall(Statement*, VariableList*, unsigned);
static void buildReturn(Statement*, VariableList*, unsigned);

static void (*buildfunctions[_statementtypemax])(Statement*, VariableList*, unsigned) =
{
    [_if] = buildIf,
    [_for] = buildFor,
    [_assignment] = buildAssignment,
    [_functioncall] = buildFunctionCall,
    [_return] = buildReturn
};

void addStatementToList(Statement *statement, StatementList **list)
{
    if(!*list)
    {
        *list = xmalloc(sizeof(*list));
        (*list)->statement = statement;
        (*list)->next = NULL;
    }

    else
    {
        StatementList *s;

        for(s = *list; s->next; s = s->next);
        s->next = xmalloc(sizeof(s->next));
        s->next->statement = statement;
        s->next->next = NULL;
    }
}

#define IS_INVALID ((type == _for && !writeableVariablesExist(scope))\
                    || (type == _assignment && !writeableVarsAvailable)\
                    || (type == _return && !nesting)\
                    || (type == _functioncall && program.numfunctions >= cmdline.max_functions))

Statement *makeRandomStatement(VariableList *scope, unsigned nesting)
{
    Statement *ret = xmalloc(sizeof(*ret));
    StatementType type;
    bool writeableVarsAvailable = writeableVariablesExist(scope);

    do
        type = statarray[rand() % 10];
    while(IS_INVALID);

    (buildfunctions[(ret->type = type)])(ret, scope, nesting);

    return ret;
}


/***** Build functions *****/

static void buildIf(Statement *statement, VariableList *scope, unsigned nesting)
{
    IfStatement *ifstatement = xmalloc(sizeof(*ifstatement));

    ifstatement->condition = makeRandomExpression(scope, 0);
    ifstatement->truepath = makeRandomBlock(scope, nesting + 1);

    /* An else branch is generated 30% of the time */
    ifstatement->falsepath = !(rand() % 3) ? makeRandomBlock(scope, nesting + 1) : NULL;

    statement->stmnt.ifstatement = ifstatement;
}

static void buildFor(Statement *statement, VariableList *scope, unsigned nesting)
{
    ForStatement *forstatement = xmalloc(sizeof(*forstatement));

    forstatement->iterator = pickRandomWriteableVariable(scope);

    /* IMPORTANT: the iterator is no more writeable, since we don't want to go into an infinite loop ! */
    forstatement->iterator->permissions &= ~P_WRITEABLE;

    /* The init var is in [-30;0[ u ]0;30] and the test val is in [30;89] */
    forstatement->init = (rand() % 30 + 1) * (VAR_IS_UNSIGNED(forstatement->iterator) ? 1 : ((rand() % 2) ? -1 : 1));
    forstatement->testval = (rand() % 60 + 30);

    /* TODO: we should produce more types of loops */
    forstatement->testop = _lowerorequal;
    forstatement->incval = 1;
    forstatement->assignop = _assigninc;

    forstatement->body = makeRandomBlock(scope, nesting + 1);

    /* Restore the write permission of the iterator */
    forstatement->iterator->permissions |= P_WRITEABLE;

    statement->stmnt.forstatement = forstatement;
}

static void buildFunctionCall(Statement *statement, VariableList *scope, unsigned nesting)
{
    FunctionCallStatement *funccallstatement = xmalloc(sizeof(*funccallstatement));
    funccallstatement->paramlist = NULL;
    VariableList *v;

    funccallstatement->function = makeRandomFunction(true);

    foreach_variable(v, funccallstatement->function->paramlist)
        addExpressionToList(makeRandomExpression(scope, nesting + 1), (ExpressionList**) &funccallstatement->paramlist);

    statement->stmnt.funccallstatement = funccallstatement;
}

static void buildAssignment(Statement *statement, VariableList *scope, unsigned nesting)
{
    AssignmentStatement *assignmentstatement = xmalloc(sizeof(*assignmentstatement));

    assignmentstatement->var = pickRandomWriteableVariable(scope);
    assignmentstatement->op = /*rand() % _assignopmax*/_assign;
    assignmentstatement->expr = makeRandomExpression(scope, 0);

    statement->stmnt.assignmentstatement = assignmentstatement;
}

static void buildReturn(Statement *statement, VariableList *scope, unsigned nesting)
{
    ReturnStatement *returnstatement = xmalloc(sizeof(*returnstatement));

    returnstatement->retval = pickRandomOperand(scope);

    statement->stmnt.returnstatement = returnstatement;
}


/***** Print functions *****/

static void printIfStatement(Statement *statement)
{
    IfStatement *ifstatement = statement->stmnt.ifstatement;

    fputs("if(", stdout);
    printExpression(ifstatement->condition);
    puts(")");
    printBlock(ifstatement->truepath);

    if(ifstatement->falsepath)
    {
        puts("else");
        printBlock(ifstatement->falsepath);
    }
}

static void printForStatement(Statement *statement)
{
    ForStatement *forstatement = statement->stmnt.forstatement;

    printf("for(%s = %d; %s %s %d; %s %s %d)\n", forstatement->iterator->name, forstatement->init,
           forstatement->iterator->name, testop2str[forstatement->testop], forstatement->testval,
           forstatement->iterator->name, assignop2str[forstatement->assignop], forstatement->incval);
    printBlock(forstatement->body);
}

static void printAssignmentStatement(Statement *statement)
{
    AssignmentStatement *assignstatement = statement->stmnt.assignmentstatement;

    printf("%s %s ", assignstatement->var->name, assignop2str[assignstatement->op]);
    printExpression(assignstatement->expr);
    puts(";");
}

static void printFunctionCallStatement(Statement *statement)
{
    ExpressionList *e;
    FunctionCallStatement *funccallstatement = statement->stmnt.funccallstatement;

    printf("\n%s(", funccallstatement->function->name);

    foreach_expression(e, funccallstatement->paramlist)
    {
        printExpression(e->expression);

        if(e->next)
            fputs(", ", stdout);
    }

    puts(");");
}

static void printReturnStatement(Statement *statement)
{
    Operand *retval = statement->stmnt.returnstatement->retval;

    printf("return %s;\n", retval->type == _variable ? retval->op.variable->name : retval->op.constant.value);
}

/* TODO: add a printX method for each type of statement ? Would be much clearer. */
void printStatement(Statement *statement)
{
    static void (*printfunctions[_statementtypemax])(Statement*) =
    {
        [_if] = printIfStatement,
        [_for] = printForStatement,
        [_return] = printReturnStatement,
        [_assignment] = printAssignmentStatement,
        [_functioncall] = printFunctionCallStatement
    };

    (printfunctions[statement->type])(statement);
}
