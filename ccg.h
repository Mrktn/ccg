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

#ifndef CCG_INCLUDED
#define CCG_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <stdarg.h>
#include <stdint.h>
#include <getopt.h>
#include <unistd.h>
#include <fcntl.h>

#include "types.h"

#define foreach_variable(v, t) for((v = (t)); (v); (v = v->next))
#define foreach_function(f, t) for((f = (t)); (f); (f = f->next))
#define foreach_statement(s, t) for((s = (t)); (s); (s = s->next))
#define foreach_expression(e, t) for((e = (t)); (e); (e = e->next))

#define VARTYPE_TO_BITNESS(type) ((type == _i8 || type == _u8 ? 8 : (type == _i16 || type == _u16 ? 16 : 32)))

#define VAR_IS_UNSIGNED(var) (var->type == _u8 || var->type == _u16 || var->type == _u32)

#define RANDOM_BITNESS (8 * pow(2, rand() % 3))

#define likely(x)       __builtin_expect((x),1)
#define unlikely(x)     __builtin_expect((x),0)

#define CCG_VERSION "0.1"

extern CommandlineOpt cmdline;
extern Program program;
extern const char *type2str[_typemax];
extern const char* testop2str[_testopmax];
extern const char* arithop2str[_arithopmax];
extern const char* assignop2str[_assignopmax];

/* Commandline proccessing */
void processCommandline(int, char**);

/* Utils */
void die(const char*, ...);
void info(const char*, ...);
void *xmalloc(size_t);
void *xcalloc(size_t, size_t);

/* Constant */
Constant makeRandomIntegerConstant(unsigned);
void printConstant(Constant*);

/* Variable */
Variable *makeRandomVariable(VariableList*);
void addVariableToList(Variable*, VariableList**);
void printQualifiers(Variable*);
void printVariable(Variable*);
void copyVariableList(VariableList*, VariableList**);
Variable *pickRandomVariable(VariableList*);
Variable *pickRandomWriteableVariable(VariableList*);
bool writeableVariablesExist(VariableList*);
void makeGlobalVariables(void);

/* Function */
Function *makeRandomFunction(bool);
void printFunction(Function*);
void printFunctionPrototype(Function*);

/* Block */
Block *makeRandomBlock(VariableList*, unsigned);
void printBlock(Block*);

/* Statement */
void addStatementToList(Statement*, StatementList**);
Statement *makeRandomStatement(VariableList*, unsigned);
void printStatement(Statement*);

/* Operand */
Operand *pickRandomOperand(VariableList*);

/* Expression */
void addExpressionToList(Expression*, ExpressionList**);
Expression *makeRandomExpression(VariableList*, unsigned);
void printExpression(Expression*);

#endif
