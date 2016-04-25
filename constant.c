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

static const char *hexdigits = "0123456789ABCDEF";

char *makeHexadecimalValue(unsigned digitNumber)
{
    size_t i;
    char *ret = xcalloc(digitNumber + 1, 1);

    for(i = 0; i < digitNumber; ++i)
        ret[i] = hexdigits[rand() % 16];

    return ret;
}

/* For now, the generated integers are strictly positive */
Constant *makeIntegerConstant(unsigned bits)
{
    Constant *ret = xmalloc(sizeof(*ret));
    char *hexvalue = ((rand() % 3) ? makeHexadecimalValue(bits / 4) : "0"); /* 30% of the time, a zero is generated */

    ret->value   = xcalloc(4 + strlen(hexvalue), 1);
    ret->bitness = bits;

    sprintf(ret->value, "0x%s", hexvalue);

    if(strlen(hexvalue) > 1)
        free(hexvalue);

    return ret;
}

void printConstant(Constant *constant)
{
    fputs(constant->value, stdout);
    /* And... we are done. */
}
