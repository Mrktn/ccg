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

#define IS_INVALID ((type == _return && !nesting)\
                    || (type == _functioncall && program.numfunctions >= cmdline.max_functions))

Statement *makeStatement(VariableList *scope, unsigned nesting)
{
    Statement *ret = xmalloc(sizeof(*ret));
    StatementType type;

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

    ifstatement->condition = makeExpression(scope, 0);
    ifstatement->truepath = makeBlock(scope, nesting + 1);

    /* An else branch is generated 30% of the time */
    ifstatement->falsepath = !(rand() % 3) ? makeBlock(scope, nesting + 1) : NULL;

    statement->stmnt.ifstatement = ifstatement;
}

static void buildFor(Statement *statement, VariableList *scope, unsigned nesting)
{
    ForStatement *forstatement = xmalloc(sizeof(*forstatement));

    forstatement->iterator = selectVariable(scope, _randomvartype);

    /* The init var is in [-30;0[ u ]0;30] and the test val is in [30;89] */
    forstatement->init = (rand() % 30 + 1) * ((forstatement->iterator->type == _integer ? IS_UNSIGNED_INTEGERTYPE(forstatement->iterator->intvar.type) : IS_UNSIGNED_INTEGERTYPE(ultimateType(forstatement->iterator))) ? 1 : ((rand() % 2) ? -1 : 1));
    forstatement->testval = (rand() % 60 + 30);

    /* TODO: we should produce more types of loops */
    forstatement->testop = _lowerorequal;
    forstatement->incval = 1;
    forstatement->assignop = _assigninc;

    forstatement->body = makeBlock(scope, nesting + 1);

    statement->stmnt.forstatement = forstatement;
}

static void buildFunctionCall(Statement *statement, VariableList *scope, unsigned nesting)
{
    FunctionCallStatement *funccallstatement = xmalloc(sizeof(*funccallstatement));
    funccallstatement->paramlist = NULL;
    VariableList *v;

    funccallstatement->function = makeFunction(true);

    foreach(v, funccallstatement->function->paramlist)
        addExpressionToList(makeExpression(scope, nesting + 1), (ExpressionList**) &funccallstatement->paramlist);

    statement->stmnt.funccallstatement = funccallstatement;
}

/* TODO: should generate pointer assignments too */
static void buildAssignment(Statement *statement, VariableList *scope, unsigned nesting)
{
    AssignmentStatement *assignmentstatement = xmalloc(sizeof(*assignmentstatement));

    assignmentstatement->var = selectVariable(scope, _randomvartype);
    assignmentstatement->op = rand() % _assignopmax /*_assign*/;
    assignmentstatement->expr = makeExpression(scope, 0);

    statement->stmnt.assignmentstatement = assignmentstatement;
}

static void buildReturn(Statement *statement, VariableList *scope, unsigned nesting)
{
    ReturnStatement *returnstatement = xmalloc(sizeof(*returnstatement));

    returnstatement->retval = selectOperand(scope);

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
    char *id = USABLE_ID(forstatement->iterator);
    
    printf("for(%s = %d; %s %s %d; %s %s %d)\n", id, forstatement->init, 
        id, testop2str[forstatement->testop], forstatement->testval,
        id, assignop2str[forstatement->assignop], forstatement->incval);

    printBlock(forstatement->body);
}

static void printAssignmentStatement(Statement *statement)
{
    AssignmentStatement *assignstatement = statement->stmnt.assignmentstatement;

    printf("%s %s ", USABLE_ID(assignstatement->var), assignop2str[assignstatement->op]);
    printExpression(assignstatement->expr);
    puts(";");
}

static void printFunctionCallStatement(Statement *statement)
{
    ExpressionList *e;
    FunctionCallStatement *funccallstatement = statement->stmnt.funccallstatement;

    printf("\n%s(", funccallstatement->function->name);

    foreach(e, funccallstatement->paramlist)
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

    printf("return %s;\n", retval->type == _variable ? USABLE_ID(retval->op.variable) : retval->op.constant.value);
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
