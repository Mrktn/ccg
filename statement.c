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


static const StatementType statarray[20] = {_if, _if, _if, _if, _for, _for, _for, _for, _assignment, _assignment, _assignment, _assignment, _ptrassignment, _ptrassignment, _ptrassignment, _functioncall, _functioncall, _functioncall, _goto, _return};

static void buildIf(Statement*, Context*, unsigned);
static void buildFor(Statement*, Context*, unsigned);
static void buildAssignment(Statement*, Context*, unsigned);
static void buildPtrAssignment(Statement*, Context*, unsigned);
static void buildFunctionCall(Statement*, Context*, unsigned);
static void buildReturn(Statement*, Context*, unsigned);
static void buildGoto(Statement*, Context*, unsigned);

static void (*buildfunctions[_statementtypemax])(Statement*, Context*, unsigned) =
{
    [_if] = buildIf,
    [_for] = buildFor,
    [_assignment] = buildAssignment,
    [_ptrassignment] = buildPtrAssignment,
    [_functioncall] = buildFunctionCall,
    [_return] = buildReturn,
    [_goto] = buildGoto
};

void addStatementToList(Statement *statement, StatementList **list)
{
    if(!*list)
    {
        *list = xmalloc(sizeof(**list));
        (*list)->statement = statement;
        (*list)->next = NULL;
    }

    else
    {
        StatementList *s;

        for(s = *list; s->next; s = s->next);
        s->next = xmalloc(sizeof(*s->next));
        s->next->statement = statement;
        s->next->next = NULL;
    }
}

#define IS_INVALID ((type == _ptrassignment && (context->nvars == context->nintegers))\
                    || (type == _functioncall && program.numfunctions >= cmdline.max_functions)\
                    || (type == _return && (!nesting || !lastofblock))\
                    || (type == _goto && (cmdline.nojumps || (context->currfunc->numlabels == 0))))

Statement *makeStatement(Context *context, unsigned nesting, bool lastofblock)
{
    Statement *ret = xmalloc(sizeof(*ret));
    StatementType type;

    do
        type = statarray[rand() % (sizeof(statarray) / sizeof(*statarray))];
    while(IS_INVALID);

    /* 2% of chance to put a label */
    if(!cmdline.nojumps && type != _goto && !(rand() % 50))
        ret->label = makeLabel(context);
    else
        ret->label = NULL;

    (buildfunctions[(ret->type = type)])(ret, context, nesting);

    return ret;
}


/***** Build functions *****/

static void buildIf(Statement *statement, Context *context, unsigned nesting)
{
    IfStatement *ifstatement = xmalloc(sizeof(*ifstatement));

    ifstatement->condition = makeExpression(context, 0);
    ifstatement->truepath = makeBlock(context, nesting + 1);

    /* An else branch is generated 30% of the time */
    ifstatement->falsepath = !(rand() % 3) ? makeBlock(context, nesting + 1) : NULL;

    statement->stmnt.ifstatement = ifstatement;
}

static void buildFor(Statement *statement, Context *context, unsigned nesting)
{
    ForStatement *forstatement = xmalloc(sizeof(*forstatement));

    forstatement->iterator = selectVariable(context, _randomvartype);

    /* The init var is in [-30;0[ u ]0;30] and the test val is in [30;89] */
    forstatement->init = (rand() % 30 + 1) * ((forstatement->iterator->type == _integer ? IS_UNSIGNED_INTEGERTYPE(forstatement->iterator->intvar.type) : IS_UNSIGNED_INTEGERTYPE(ultimateType(forstatement->iterator))) ? 1 : ((rand() % 2) ? -1 : 1));
    forstatement->testval = (rand() % 60 + 30);

    forstatement->testop = _lowerorequal;
    forstatement->incval = 1;
    forstatement->assignop = _assigninc;

    forstatement->body = makeBlock(context, nesting + 1);

    statement->stmnt.forstatement = forstatement;
}

static void buildFunctionCall(Statement *statement, Context *context, unsigned nesting)
{
    FunctionCallStatement *funccallstatement = xmalloc(sizeof(*funccallstatement));
    funccallstatement->paramlist = NULL;
    VariableList *v;

    funccallstatement->function = makeFunction(true);

    foreach(v, funccallstatement->function->paramlist)
        addExpressionToList(makeExpression(context, nesting + 1), (ExpressionList**) &funccallstatement->paramlist);

    statement->stmnt.funccallstatement = funccallstatement;
}

static void buildAssignment(Statement *statement, Context *context, unsigned nesting)
{
    AssignmentStatement *as = xmalloc(sizeof(*as));

    as->var = selectVariable(context, _randomvartype);
    as->op = rand() % _assignopmax /*_assign*/;
    as->expr = makeExpression(context, 0);

    statement->stmnt.assignmentstatement = as;
}

#define PTRASSIGNMENT_IS_CONSISTENT(lhs, rhs) (INTEGERTYPE_SIZE(ultimateType(lhs)) <= INTEGERTYPE_SIZE(ultimateType(rhs)))

static void buildPtrAssignment(Statement *statement, Context *context, unsigned nesting)
{
    Variable *v;
    PtrAssignmentStatement *pas = xmalloc(sizeof(*pas));

    pas->lhs = selectVariable(context, _pointer);

    do
        v = selectVariable(context, _randomvartype);
    while(!PTRASSIGNMENT_IS_CONSISTENT(pas->lhs, v));

    pas->rhs = v;

    statement->stmnt.ptrassignmentstatement = pas;
}

static void buildReturn(Statement *statement, Context *context, unsigned nesting)
{
    ReturnStatement *rs = xmalloc(sizeof(*rs));

    rs->retval = selectOperand(context);

    statement->stmnt.returnstatement = rs;
}

static void buildGoto(Statement *statement, Context *context, unsigned nesting)
{
    GotoStatement *gs = xmalloc(sizeof(*gs));

    gs->label = selectLabel(context);

    statement->stmnt.gotostatement = gs;
}


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

static void printPtrAssignmentStatement(Statement *statement)
{
    PtrAssignmentStatement *pas = statement->stmnt.ptrassignmentstatement;
    size_t lhsdepth = pointerDepth(pas->lhs), rhsdepth = pointerDepth(pas->rhs);

    if(pas->rhs->type == _pointer)
    {
        if(lhsdepth == rhsdepth)
            printf("%s = %s;\n", pas->lhs->name, pas->rhs->name);
        else if(lhsdepth < rhsdepth)
            printf("%s = %s%s;\n", pas->lhs->name, genStars(rhsdepth - lhsdepth), pas->rhs->name);
        else
            printf("%s%s = %s;\n", genStars(lhsdepth - rhsdepth), pas->lhs->name, pas->rhs->name);
    }

    else
    {
        printf("%s%s = &%s;\n", genStars(lhsdepth - rhsdepth - 1), pas->lhs->name, pas->rhs->name);
    }
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

    printf("return %s;\n", retval->type == _variable ? USABLE_ID(retval->op.variable) : retval->op.constant->value);
}

static void printGotoStatement(Statement *statement)
{
    printf("goto %s;\n", statement->stmnt.gotostatement->label->name);
}

void printStatement(Statement *statement)
{
    static void (*printfunctions[_statementtypemax])(Statement*) =
    {
        [_if] = printIfStatement,
        [_for] = printForStatement,
        [_return] = printReturnStatement,
        [_assignment] = printAssignmentStatement,
        [_ptrassignment] = printPtrAssignmentStatement,
        [_functioncall] = printFunctionCallStatement,
        [_goto] = printGotoStatement
    };

    if(statement->label)
        printf("%s:\n", statement->label->name);

    (printfunctions[statement->type])(statement);
}
