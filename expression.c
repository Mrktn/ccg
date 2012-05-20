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
	- Ternary: 10%
	- Function call: 10%
	- Operation: 20%
	- Test: 20%
	- Assignment: 20%
	- Operand: 20%
*/
static const ExpressionType exprarray[10] = {_ternaryexpr, _functioncallexpr, _operationexpr, _operationexpr, _testexpr, _testexpr, _assignmentexpr, _assignmentexpr};

const char *testop2str[_testopmax] = {"==", "<=", ">=", "<", ">", "!="};
const char *arithop2str[_arithopmax] = {"+", "-", "/", "*"};
const char *bitwiseop2str[_bitwiseopmax] = {"&", "|", "^"};
const char *logicalop2str[_logicalopmax] = {"&&", "||"};
const char *assignop2str[_assignopmax] = {"+=", "-=", "/=", "*=", "="};

void buildOperand(Expression*, VariableList*, unsigned);
void buildTernary(Expression *expr, VariableList*, unsigned);
void buildOperation(Expression*, VariableList*, unsigned);
void buildTest(Expression*, VariableList*, unsigned);
void buildAssignment(Expression*, VariableList*, unsigned);
void buildFunctionCall(Expression*, VariableList*, unsigned);

static void (*buildfunctions[_expressiontypemax])(Expression*, VariableList*, unsigned) =
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
        *list = xmalloc(sizeof(*list));
        (*list)->expression = expression;
        (*list)->next = NULL;
    }

    else
    {
        ExpressionList *e;

        for(e = *list; e->next; e = e->next);
        e->next = xmalloc(sizeof(e->next));
        e->next->expression = expression;
        e->next->next = NULL;
    }
}

#define EXPRESSION_IS_INVALID ((expression->type == _assignmentexpr && !writeableVarsAvailable)\
                            || (expression->type == _functioncallexpr && program.numfunctions >= cmdline.max_functions))

Expression *makeRandomExpression(VariableList *scope, unsigned nesting)
{
	Expression *expression = xmalloc(sizeof(*expression));
	bool writeableVarsAvailable = writeableVariablesExist(scope);

	/* If we want to put en end to the expression nesting, we have to put an operand, either a variable or a constant */
	if(nesting >= cmdline.max_expression_nesting)
		expression->type = _operandexpr;
	else
	{
		do
			expression->type = exprarray[rand() % 10];
		while(EXPRESSION_IS_INVALID);
	}

	(buildfunctions[expression->type])(expression, scope, nesting + 1);

	return expression;
}

void buildOperand(Expression *expression, VariableList *scope, unsigned nesting)
{
	expression->expr.operand = pickRandomOperand(scope);
}

void buildTest(Expression *expression, VariableList *scope, unsigned nesting)
{
	struct TestExpression *te = xmalloc(sizeof(*te));

	te->op = rand() % _testopmax;
	te->lefthand = makeRandomExpression(scope, nesting + 1), te->righthand = makeRandomExpression(scope, nesting + 1);

	expression->expr.testexpr = te;
}

void buildTernary(Expression *expression, VariableList *scope, unsigned nesting)
{
	struct TernaryExpression *te = xmalloc(sizeof(*te));

	/* Build the test part of the ternary */
	te->test = makeRandomExpression(scope, nesting + 1);
	te->truepath = makeRandomExpression(scope, nesting + 1), te->falsepath = makeRandomExpression(scope, nesting + 1);
	expression->expr.ternexpr = te;
}

#define IS_FLOATING_POINT_VARIABLE(expression) ((expression->type == _operandexpr)\
                                                && (expression->expr.operand->type == _variable)\
                                                && ((expression->expr.operand->op.variable->type == _double) || (expression->expr.operand->op.variable->type == _float)))\

#define OPERATION_OP_IS_INVALID(oprtr, left, right) (((IS_FLOATING_POINT_VARIABLE(left)) || (IS_FLOATING_POINT_VARIABLE(right)))\
														&& (oprtr == _mod))

void buildOperation(Expression *expression, VariableList *scope, unsigned nesting)
{
	struct OperationExpression *oe = xmalloc(sizeof(*oe));

	oe->lefthand = makeRandomExpression(scope, nesting + 1), oe->righthand = makeRandomExpression(scope, nesting + 1);
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

void buildAssignment(Expression *expression, VariableList *scope, unsigned nesting)
{
	struct AssignmentExpression *ae = xmalloc(sizeof(*ae));

	if(!(ae->lvalue = pickRandomWriteableVariable(scope)))
		die("you kidding");

	ae->rvalue = makeRandomExpression(scope, nesting + 1);
	ae->op = rand() % _assignopmax;

	expression->expr.assignexpr= ae;
}

void buildFunctionCall(Expression *expression, VariableList *scope, unsigned nesting)
{
	struct FunctionCallExpression *fce = xmalloc(sizeof(*fce));
	VariableList *v;

	fce->paramlist = NULL;
	fce->function = makeRandomFunction(true);

	foreach_variable(v, fce->function->paramlist)
		addExpressionToList(makeRandomExpression(scope, nesting + 1), (ExpressionList**) &fce->paramlist);

	expression->expr.funccallexpr = fce;
}

static void printOperand(Operand *op)
{
	if(op->type == _variable)
		fputs(op->op.variable->name, stdout);
	else
		printConstant(&op->op.constant);
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
	printf("%s %s ", ae->lvalue->name, assignop2str[ae->op]);
	printExpression(ae->rvalue);
	putchar(')');
}

static void printFunctionCall(struct FunctionCallExpression *fce)
{
	ExpressionList *e;

	printf("%s(", fce->function->name);

    foreach_expression(e, fce->paramlist)
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