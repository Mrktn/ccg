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

Program program;

/* TODO: make it portable across Windows */
static unsigned getseed(void)
{
    int urandom = open("/dev/urandom", O_RDONLY);
    unsigned seed;
    read(urandom, &seed, sizeof(unsigned));
    close(urandom);

    return seed;
}

static void init(void)
{
    srand(getseed());
    program.globalvars = NULL, program.functions = NULL;
    program.numfunctions = program.numglobalvars = 0;

    /* Default settings */
    cmdline.max_functions = 15;
    cmdline.max_localvars = 4;
    cmdline.max_function_parameters = 4;
    cmdline.max_statements_per_block = 7;
    cmdline.max_expression_nesting = 8;
    cmdline.max_block_nesting = 7;
}

static void printProgram(void)
{
    FunctionList *f;
    VariableList *v;

    puts("#include <stdint.h>");
    puts("#include <stdlib.h>\n\n/* Global variables */");

    foreach_variable(v, program.globalvars)
    {
        printVariable(v->variable);
    }

    puts("\n/* Function prototypes */");

    foreach_function(f, program.functions)
    {
        printFunctionPrototype(f->function);
        puts(";");
    }

    putchar('\n');

    foreach_function(f, program.functions)
    {
        printFunction(f->function);
    }

    puts("\nint main(void)\n{\n"
         "func_0();\n"
         "return 0;\n}");
}

int main(int argc, char **argv)
{
    init();
    processCommandline(argc, argv);
    makeGlobalVariables();
    makeRandomFunction(false);
    printProgram();

    return EXIT_SUCCESS;
}
