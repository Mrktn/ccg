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

#define P_WRITEABLE (1 << 0)

typedef int bool;

#define true  1
#define false 0

typedef enum {_i8, _u8, _i16, _u16, _i32, _u32, _i64, _u64, _inttypemax} IntegerType;
typedef enum {_if, _for, _functioncall, _return, _assignment, _ptrassignment, _goto, _statementtypemax} StatementType;
typedef enum {_variable, _constant, _operandtypemax, _none = 42} OperandType;
typedef enum {_equal, _lowerorequal, _greaterorequal, _lower, greater, _different, _testopmax} TestOp;
typedef enum {_plus, _minus, _div, _mod, _mul, _arithopmax} ArithOp;
typedef enum {_bwand, _bwor, _xor, _bitwiseopmax} BitwiseOp;
typedef enum {_logand, _logor, _logicalopmax} LogicalOp;
typedef enum {_arithmetic, _bitwise, _logical, _operationtypemax} OperationType;
typedef enum {_operandexpr, _ternaryexpr, _operationexpr, _testexpr, _assignmentexpr, _functioncallexpr, _expressiontypemax} ExpressionType;
typedef enum {_assigninc, _assigndec_, _assigndiv, _assignmod, _assignmul, _assignand, _assignor, _assignxor, _assign, _assignopmax} AssignmentOp;

typedef enum {_integer, _pointer, _vartypemax, _randomvartype = 42} VariableType;

typedef uint8_t Permissions;

#pragma pack(4)
typedef struct
{
    char *value;
    unsigned bitness;
} Constant;

typedef struct Variable
{
    char *name;

    union
    {
        struct IntegerVariable
        {
            Constant *initializer;
            IntegerType type;
        } intvar;

        struct Pointer
        {
            struct Variable *pointed;
        } pointer;
    };

    VariableType type;
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
        Constant *constant;
    } op;
} Operand;

typedef struct
{
    void *condition; /* Expression *condition */

    struct BLCK
    {
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
    void *expr;
    AssignmentOp op;
} AssignmentStatement;

typedef struct
{
    Variable *lhs, *rhs;
} PtrAssignmentStatement;

typedef struct
{
    Operand *retval;
} ReturnStatement;

typedef struct
{
    char *name;
} Label;

typedef struct LL
{
    Label *label;
    struct LL *next;
} LabelList;

typedef struct
{
    char *name;
    IntegerType returntype;
    VariableList *paramlist;
    Block *body;
    LabelList *labels;
    short numlabels;
} Function;

typedef struct
{
    Function *function;
    void *paramlist; /* ExpressionList *paramlist; */
} FunctionCallStatement;

typedef struct
{
    Label *label;
} GotoStatement;

typedef struct Stmnt
{
    StatementType type;
    Label *label;

    union
    {
        IfStatement *ifstatement;
        WhileStatement *whilestatement;
        ForStatement *forstatement;
        FunctionCallStatement *funccallstatement;
        AssignmentStatement *assignmentstatement;
        PtrAssignmentStatement *ptrassignmentstatement;
        ReturnStatement *returnstatement;
        GotoStatement *gotostatement;
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

typedef struct Context
{
    unsigned short nvars, nintegers;
    VariableList *scope;
    Function *currfunc;
} Context;

typedef struct
{
    VariableList *globalvars;
    FunctionList *functions;
    size_t numfunctions, numglobalvars;
} Program;

typedef struct
{
    unsigned seed, max_functions, max_localvars, max_function_parameters, max_statements_per_block,
    max_expression_nesting, max_block_nesting, max_pointer_depth;
    bool nojumps;
} CommandlineOpt;
