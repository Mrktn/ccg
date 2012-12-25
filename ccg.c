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

Program program;

#ifdef DEBUG_MEMORY
extern unsigned long long int allocated_bytes;
#endif

static unsigned getseed(void)
{
    unsigned seed;
    int urandom = open("/dev/urandom", O_RDONLY);

    if(urandom == -1)
        die("couldn't open /dev/urandom for reading.");

    read(urandom, &seed, sizeof(unsigned));
    close(urandom);

    return seed;
}

static void init(void)
{
    program.globalvars = NULL, program.functions = NULL;
    program.numfunctions = program.numglobalvars = 0;

    /* Default settings */
    cmdline.max_functions = 15;
    cmdline.max_localvars = 4;
    cmdline.max_function_parameters = 4;
    cmdline.max_statements_per_block = 7;
    cmdline.max_expression_nesting = 8;
    cmdline.max_block_nesting = 7;
    cmdline.max_pointer_depth = 2;
    cmdline.seed = getseed();
    cmdline.nojumps = false;
}

static void printProgram(void)
{
    FunctionList *f;
    VariableList *v;

    printf("/* Seed: %d */\n", cmdline.seed);

    puts("#include <stdint.h>");
    puts("#include <stdlib.h>\n\n/* Global variables */");

    foreach(v, program.globalvars)
        printVariableDecl(v->variable);

    puts("\n/* Function prototypes */");

    foreach(f, program.functions)
    {
        printFunctionPrototype(f->function);
        puts(";");
    }

    putchar('\n');

    foreach(f, program.functions)
        printFunction(f->function);

    puts("\nint main(void)\n{\n"
         "func_0();\n"
         "return 0;\n}");
}

int main(int argc, char **argv)
{
    init();
    processCommandline(argc, argv);
    srand(cmdline.seed);
    makeGlobalVariables();
    makeFunction(false);
    printProgram();

#ifdef DEBUG_MEMORY
    fprintf(stderr, "Allocated bytes : %llu.\n", allocated_bytes);
#endif

    return EXIT_SUCCESS;
}
