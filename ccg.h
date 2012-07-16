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

#if defined __clang__
 #if __clang_major__ < 3 || (__clang_major__ == 3 && __clang_minor__ < 1)
 # error Clang 3.1 or newer is needed for C11 anonymous structs and unions
 #endif
#elif defined __GNUC__
 #define GCC_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 +__GNUC_PATCHLEVEL__)
 #if GCC_VERSION < 40400
 # error GCC 4.4 or newer is needed for C11 anonymous structs and unions
 #endif
#else
 #warning Your compiler might not support C11 anonymous structs and unions
#endif

#include "types.h"

#define foreach(v, t) for((v = (t)); (v); (v = v->next))

#define INTEGERTYPE_SIZE(type) ((type == _i8 || type == _u8 ? 8 : (type == _i16 || type == _u16 ? 16 : 32)))

#define IS_UNSIGNED_INTEGERTYPE(type) (type == _u8 || type == _u16 || type == _u32)

#define RANDOM_BITNESS (8 * pow(2, rand() % 3))

#define USABLE_ID(var) (var->type == _pointer ? maxDerefdPointer(var) : var->name)


#define CCG_VERSION "0.2"

extern CommandlineOpt cmdline;
extern Program program;
extern char const * const inttype2str[_inttypemax];
extern char const * const testop2str[_testopmax];
extern char const * const arithop2str[_arithopmax];
extern char const * const assignop2str[_assignopmax];

/* Commandline proccessing */
void processCommandline(int, char**);

/* Utils */
void die(const char*, ...);
void info(const char*, ...);
void *xmalloc(size_t);
void *xcalloc(size_t, size_t);
char *genStars(size_t n);

/* Constant */
Constant makeIntegerConstant(unsigned);
void printConstant(Constant*);

/* Variable */
Variable *makeVariable(VariableList*, VariableType);
void addVariableToList(Variable*, VariableList**);
void printVariableDecl(Variable*);
void printVariableType(Variable*);
void copyVariableList(VariableList*, VariableList**);
Variable *selectVariable(VariableList*, VariableType);
void makeGlobalVariables(void);
IntegerType ultimateType(Variable*);
size_t numVariablesInScope(VariableList*);

/* Pointer */
void makePointer(Variable*, VariableList*);
Variable *pickPointableVariable(VariableList*);
size_t pointerDepth(Variable*);
bool pointersInScope(VariableList*);
char *maxDerefdPointer(Variable*);
void printPointerDecl(Variable*);

/* Integer */
void makeInteger(Variable*, VariableList*);
bool writeableIntegersExist(VariableList*);
void printIntegerDecl(Variable*);

/* Function */
Function *makeFunction(bool);
void printFunction(Function*);
void printFunctionPrototype(Function*);

/* Block */
Block *makeBlock(VariableList*, unsigned);
void printBlock(Block*);

/* Statement */
void addStatementToList(Statement*, StatementList**);
Statement *makeStatement(VariableList*, unsigned);
void printStatement(Statement*);

/* Operand */
Operand *selectOperand(VariableList*);

/* Expression */
void addExpressionToList(Expression*, ExpressionList**);
Expression *makeExpression(VariableList*, unsigned);
void printExpression(Expression*);

#endif
