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

static const ExpressionType exprarray[10] = {_ternaryexpr, _functioncallexpr, _functioncallexpr, _operationexpr, _operationexpr, _operationexpr, _testexpr, _testexpr, _assignmentexpr, _assignmentexpr};

char const * const testop2str[_testopmax] = {"==", "<=", ">=", "<", ">", "!="};
char const * const arithop2str[_arithopmax] = {"+", "-", "/", "%", "*"};
char const * const bitwiseop2str[_bitwiseopmax] = {"&", "|", "^"};
char const * const logicalop2str[_logicalopmax] = {"&&", "||"};
char const * const assignop2str[_assignopmax] = {"+=", "-=", "/=", "%=", "*=", "&=", "|=", "^=", "="};

void buildOperand(Expression*, Context*, unsigned);
void buildTernary(Expression *expr, Context*, unsigned);
void buildOperation(Expression*, Context*, unsigned);
void buildTest(Expression*, Context*, unsigned);
void buildAssignment(Expression*, Context*, unsigned);
void buildFunctionCall(Expression*, Context*, unsigned);

static void (*buildfunctions[_expressiontypemax])(Expression*, Context*, unsigned) =
{
    [_operandexpr] = buildOperand,
    [_ternaryexpr] = buildTernary,
    [_operationexpr] = buildOperation,
    [_testexpr] = buildTest,
    [_assignmentexpr] = buildAssignment,
    [_functioncallexpr] = buildFunctionCall
};

void addExpressionToList(Expression *expression, ExpressionList **list)
{
    if(!*list)
    {
        *list = xmalloc(sizeof(**list));
        (*list)->expression = expression;
        (*list)->next = NULL;
    }

    else
    {
        ExpressionList *e;

        for(e = *list; e->next; e = e->next);
        e->next = xmalloc(sizeof(*e->next));
        e->next->expression = expression;
        e->next->next = NULL;
    }
}

#define EXPRESSION_IS_INVALID(t) (((t) == _functioncallexpr && program.numfunctions >= cmdline.max_functions))

Expression *makeExpression(Context *context, unsigned nesting)
{
    Expression *expression = xmalloc(sizeof(*expression));

    if(nesting >= cmdline.max_expression_nesting)
        expression->type = _operandexpr;
    else
    {
        do
            expression->type = exprarray[rand() % (sizeof(exprarray) / sizeof(*exprarray))];
        while(EXPRESSION_IS_INVALID(expression->type));
    }

    (buildfunctions[expression->type])(expression, context, nesting + 1);

    return expression;
}

void buildOperand(Expression *expression, Context *context, unsigned nesting)
{
    expression->expr.operand = selectOperand(context);
}

void buildTest(Expression *expression, Context *context, unsigned nesting)
{
    struct TestExpression *te = xmalloc(sizeof(*te));

    te->op = rand() % _testopmax;
    te->lefthand = makeExpression(context, nesting + 1), te->righthand = makeExpression(context, nesting + 1);

    expression->expr.testexpr = te;
}

void buildTernary(Expression *expression, Context *context, unsigned nesting)
{
    struct TernaryExpression *te = xmalloc(sizeof(*te));

    te->test = makeExpression(context, nesting + 1);
    te->truepath = (rand() % 4) ? makeExpression(context, nesting + 1) : NULL, te->falsepath = makeExpression(context, nesting + 1);
    expression->expr.ternexpr = te;
}

void buildOperation(Expression *expression, Context *context, unsigned nesting)
{
    struct OperationExpression *oe = xmalloc(sizeof(*oe));

    oe->lefthand = makeExpression(context, nesting + 1), oe->righthand = makeExpression(context, nesting + 1);
    oe->type = rand() % _operationtypemax;

    if(oe->type == _arithmetic)
        oe->operator.arithop = rand() % _arithopmax;
    else if(oe->type == _bitwise)
        oe->operator.bitwiseop = rand() % _bitwiseopmax;
    else
        oe->operator.logicalop = rand() % _logicalopmax;

    expression->expr.opexpr = oe;
}

#define ASSIGNMENT_OP_IS_INVALID(oprtr, left, right) ((((left->type == _double || left->type == _float) || (IS_FLOATING_POINT_VARIABLE(right))) && (oprtr == _assignmod)))

void buildAssignment(Expression *expression, Context *context, unsigned nesting)
{
    struct AssignmentExpression *ae = xmalloc(sizeof(*ae));

    ae->lvalue = selectVariable(context, _randomvartype);
    ae->rvalue = makeExpression(context, nesting + 1);
    ae->op = rand() % _assignopmax;

    expression->expr.assignexpr= ae;
}

void buildFunctionCall(Expression *expression, Context *context, unsigned nesting)
{
    struct FunctionCallExpression *fce = xmalloc(sizeof(*fce));
    VariableList *v;

    fce->paramlist = NULL;
    fce->function = makeFunction(true);

    foreach(v, fce->function->paramlist)
        addExpressionToList(makeExpression(context, nesting + 1), (ExpressionList**) &fce->paramlist);

    expression->expr.funccallexpr = fce;
}

static void printOperand(Operand *op)
{
    if(op->type == _variable)
        fputs(USABLE_ID(op->op.variable), stdout);
    else
        printConstant(op->op.constant);
}

void printTest(struct TestExpression *te)
{
    putchar('(');
    printExpression(te->lefthand);
    printf(" %s ", testop2str[te->op]);
    printExpression(te->righthand);
    putchar(')');
}

static void printTernary(struct TernaryExpression *te)
{
    putchar('(');
    printExpression(te->test);
    fputs(" ? ", stdout);

    if(te->truepath)
        printExpression(te->truepath);

    fputs(" : ", stdout);
    printExpression(te->falsepath);
    putchar(')');
}

static void printOperation(struct OperationExpression *oe)
{
    putchar('(');
    printExpression(oe->lefthand);
    printf(" %s ", oe->type == _arithmetic ? arithop2str[oe->operator.arithop] : (oe->type == _bitwise ? bitwiseop2str[oe->operator.bitwiseop] : logicalop2str[oe->operator.logicalop]));
    printExpression(oe->righthand);
    putchar(')');
}

static void printAssignment(struct AssignmentExpression *ae)
{
    putchar('(');
    printf("%s %s ", USABLE_ID(ae->lvalue), assignop2str[ae->op]);
    printExpression(ae->rvalue);
    putchar(')');
}

static void printFunctionCall(struct FunctionCallExpression *fce)
{
    ExpressionList *e;

    printf("%s(", fce->function->name);

    foreach(e, fce->paramlist)
    {
        printExpression(e->expression);

        if(e->next)
            fputs(", ", stdout);
    }

    putchar(')');
}

void printExpression(Expression *expression)
{
    switch(expression->type)
    {
        case _operandexpr: printOperand(expression->expr.operand); break;
        case _ternaryexpr: printTernary(expression->expr.ternexpr); break;
        case _operationexpr: printOperation(expression->expr.opexpr); break;
        case _testexpr: printTest(expression->expr.testexpr); break;
        case _assignmentexpr: printAssignment(expression->expr.assignexpr); break;
        case _functioncallexpr: printFunctionCall(expression->expr.funccallexpr); break;
        default: die("are you... mad ?");
    }
}
