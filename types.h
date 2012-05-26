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

#define P_WRITEABLE (1 << 0)

typedef int bool;

#define true  1
#define false 0

typedef enum {_i8, _u8, _i16, _u16, _i32, _u32, _typemax} VariableType;
typedef enum {_if, _for, _functioncall, _return, _assignment, _statementtypemax} StatementType;
typedef enum {_variable, _constant, _operandtypemax, _none = 42} OperandType;
typedef enum {_equal, _lowerorequal, _greaterorequal, _lower, greater, _different, _testopmax} TestOp;
typedef enum {_plus, _minus, _div, _mul, _arithopmax} ArithOp;
typedef enum {_bwand, _bwor, _xor, _bitwiseopmax} BitwiseOp;
typedef enum {_logand, _logor, _logicalopmax} LogicalOp;
typedef enum {_arithmetic, _bitwise, _logical, _operationtypemax} OperationType;
typedef enum {_operandexpr, _ternaryexpr, _operationexpr, _testexpr, _assignmentexpr, _functioncallexpr, _expressiontypemax} ExpressionType;
typedef enum {_assigninc, _assigndec_, _assigndiv, _assignmul, _assign, _assignopmax} AssignmentOp;

typedef uint8_t Permissions;

typedef struct
{
    char *value;
    unsigned bitness;
} Constant;

typedef struct
{
    char *name;
    Constant initializer;
    VariableType type;
    Permissions permissions;
} Variable;

typedef struct VL
{
    Variable *variable;
    struct VL *next;
} VariableList;

typedef struct
{
    OperandType type;

    union
    {
        Variable *variable;
        Constant constant;
    } op;
} Operand;

typedef struct
{
    void *condition; /* Expression *condition */

    struct BLCK
    {
        unsigned numlocalvars, numstatements;
        VariableList *localvars;

        struct SL
        {
            struct Stmnt *statement;
            struct SL *next;
        } *statementlist;
    } *truepath, *falsepath;
} IfStatement;

typedef struct SL StatementList;
typedef struct BLCK Block;

typedef struct
{
    void *condition;
    Block *body;
} WhileStatement;

/* for(iterator = init; iterator testop val; iterator assignop incval) */
typedef struct
{
    Variable *iterator;
    int init, testval, incval;
    TestOp testop;
    AssignmentOp assignop;
    Block *body;
} ForStatement;

typedef struct
{
    Variable *var;
    void *expr;    /* This is an Expression */
    AssignmentOp op;
} AssignmentStatement;

typedef struct
{
    Operand *retval;
} ReturnStatement;

typedef struct
{
    char *name;
    VariableType returntype;
    short numparams;
    VariableList *paramlist;
    Block *body;
} Function;

typedef struct
{
    Function *function;
    void *paramlist; /* ExpressionList *paramlist; */
} FunctionCallStatement;

typedef struct Stmnt
{
    StatementType type;

    union
    {
        IfStatement *ifstatement;
        WhileStatement *whilestatement;
        ForStatement *forstatement;
        FunctionCallStatement *funccallstatement;
        AssignmentStatement *assignmentstatement;
        ReturnStatement *returnstatement;
    } stmnt;
} Statement;

typedef struct FL
{
    Function *function;
    struct FL *next;
} FunctionList;

typedef struct Expr
{
    ExpressionType type;

    /*
    	An expression, here, is something that returns a value. It can be any of:
    		* A boolean expression, for instance (var == 0x42)
    		* A ternary expression, for instance (var == var2 ? 0x42 : othervar)
    		* An arithmetic operation, for instance (var / 3)
    		* A simple operand, an operand being the simplest item we manipulate (that is to say a variable or a integer constant)
    		* The value returned by a function call
    */
    union
    {
        struct TestExpression
        {
            struct Expr *lefthand, *righthand;
            TestOp op;
        } *testexpr;

        struct TernaryExpression
        {
            struct Expr *test;
            struct Expr *truepath, *falsepath;
        } *ternexpr;

        struct OperationExpression
        {
            struct Expr *lefthand, *righthand;
            OperationType type;

            union
            {
                ArithOp arithop;
                BitwiseOp bitwiseop;
                LogicalOp logicalop;
            } operator;
        } *opexpr;

        struct AssignmentExpression
        {
            Variable *lvalue;
            struct Expr *rvalue;
            AssignmentOp op;
        } *assignexpr;

        struct FunctionCallExpression
        {
            Function *function;
            void *paramlist; /* struct ExpressionList *paramlist */
        } *funccallexpr;

        Operand *operand;
    } expr;
} Expression;

typedef struct EL
{
    Expression *expression;
    struct EL *next;
} ExpressionList;

typedef struct
{
    Variable **scope;
    Function *currfunc;
    unsigned nvars;
} Context;

typedef struct
{
    VariableList *globalvars;
    FunctionList *functions;
    short numfunctions, numglobalvars;
} Program;

typedef struct
{
    unsigned seed, max_functions, max_localvars, max_function_parameters, max_statements_per_block, max_expression_nesting, max_block_nesting;
} CommandlineOpt;