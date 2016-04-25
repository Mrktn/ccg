/* C Code Generator
 *
 * Copyright (C) 2016, Antoine Balestrat <antoine.balestrat@gmail.com>
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

extern char *optarg;

CommandlineOpt cmdline;

static void printHelp(void)
{
    puts("  -h, --help\t\t\tShow this help");
    puts("  -v, --version\t\t\tPrint the version of CCG");
    puts("  --seed\t\t\tUse a custom seed (instead of a random one)");
    puts("  --max-functions\t\tMax number of functions to be generated (default 12)");
    puts("  --max-localvars\t\tMax number of variables in a block (default 4)");
    puts("  --max-function-parameters\tMax number of parameters for a function (default 4)");
    puts("  --max-statements-per-block\tMax number of statements in each block (default 7)");
    puts("  --max-expression-nesting\tControl the complexity of the expressions (default 8)");
    puts("  --max-block-nesting\t\tControl the nesting of the blocks (default 7)");
    puts("  --max-pointer-depth\t\tMaximum depth of a pointer (default 2)");
    puts("  --no-jumps\t\t\tDisables jumps (enabled by default)");
    exit(EXIT_SUCCESS);
}

static void printVersion(void)
{
    puts("This is ccg version " CCG_VERSION);
    exit(EXIT_SUCCESS);
}

static void setopt(int index)
{
    unsigned * const index2member[] =
    {
        &cmdline.seed, &cmdline.max_functions, &cmdline.max_localvars, &cmdline.max_function_parameters,
        &cmdline.max_statements_per_block, &cmdline.max_expression_nesting, &cmdline.max_block_nesting, &cmdline.max_pointer_depth
    };

    if(index >= 0 && index <= 7)
        *(index2member[index]) = strtol(optarg, NULL, 10);
    else if(index == 8)
        cmdline.nojumps = true;
    else if(index == 9)
        printHelp();
    else if(index == 10)
        printVersion();

    /* Sanity check */
    if(cmdline.max_pointer_depth == 0)
        die("the maximum pointer depth can't be 0 !");
}

void processCommandline(int argc, char **argv)
{
    int c, index = 0;

    const struct option longopt[] =
    {
        {"seed", required_argument, NULL, 0},
        {"max-functions", required_argument, NULL, 0},
        {"max-localvars", required_argument, NULL, 0},
        {"max-function-parameters", required_argument, NULL, 0},
        {"max-statements-per-block", required_argument, NULL, 0},
        {"max-expression-nesting", required_argument, NULL, 0},
        {"max-block-nesting", required_argument, NULL, 0},
        {"max-pointer-depth", required_argument, NULL, 0},
        {"no-jumps", no_argument, NULL, 0},
        {"help", no_argument, NULL, 0},
        {"version", no_argument, NULL, 0},
        {NULL, 0, NULL, 0}
    };

    while((c = getopt_long(argc, argv, "hv", longopt, &index)) != -1)
    {
        switch(c)
        {
            case 0:
                setopt(index);
                break;

            case 'h':
                printHelp();
                break;

            case 'v':
                printVersion();
                break;

            default:
                break;
        }
    }
}
