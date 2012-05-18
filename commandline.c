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

extern char *optarg;

CommandlineOpt cmdline;

static void printHelp()
{
    puts("    -h, --help\t\t\t\tShow this help");
    puts("    -v, --version\t\t\tPrint the version of CCG");
    puts("    --max-functions\t\t\tMax number of functions to be generated (default 12)");
    puts("    --max-localvars\t\t\tMax number of variables in a block (default 4)");
    puts("    --max-function-parameters\t\tMax number of parameters for a function (default 4)");
    puts("    --max-statements-per-block\t\tMax number of statements in each block (default 7)");
    puts("    --max-expression-nesting\t\tControl the complexity of the expressions (default 8)");
    puts("    --max-block-nesting\t\tControl the nesting of the blocks (default 7)");
    exit(EXIT_SUCCESS);
}

static void printVersion()
{
    puts("This is ccg version " CCG_VERSION);
    exit(EXIT_SUCCESS);
}

static void setopt(int index)
{
    short * const index2member[] =
    {
        &cmdline.max_functions, &cmdline.max_localvars, &cmdline.max_function_parameters,
        &cmdline.max_statements_per_block, &cmdline.max_expression_nesting, &cmdline.max_block_nesting
    };

    if(index >= 0 && index <= 5)
        *(index2member[index]) = strtol(optarg, NULL, 10);
    else if(index == 6)
        printHelp();
    else if(index == 7)
        printVersion();
}

void processCommandline(int argc, char **argv)
{
    int i, c, index = 0;

    const struct option longopt[] =
    {
    	{"max-functions", required_argument, NULL, 0},
    	{"max-localvars", required_argument, NULL, 0},
    	{"max-function-parameters", required_argument, NULL, 0},
    	{"max-statements-per-block", required_argument, NULL, 0},
        {"max-expression-nesting", required_argument, NULL, 0},
        {"max-block-nesting", required_argument, NULL, 0},
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
