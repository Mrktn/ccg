/*	C Code Generator
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

static const char *hexdigits = "0123456789ABCDEF";

static char *makeRandomHexadecimalValue(unsigned digitNumber)
{
    size_t i = 0;
    char *ret = xcalloc(digitNumber + 1, 1);

    for(; i < digitNumber; ++i)
        ret[i] = hexdigits[rand() % 16];

    return ret;
}

/* For now, the generated integers are strictly positive */
Constant makeRandomIntegerConstant(unsigned bits)
{
    Constant ret;
    char *hexvalue = ((rand() % 3) ? makeRandomHexadecimalValue(bits / 4) : "0"); /* 30% of the time, a zero is generated */

	ret.value   = xcalloc(4 + strlen(hexvalue), 1);
	ret.bitness = bits;

    sprintf(ret.value, "0x%s", hexvalue);

    if(strlen(hexvalue) > 1)
        free(hexvalue);

    return ret;
}

void printConstant(Constant *constant)
{
    fputs(constant->value, stdout);
    /* And... we are done. */
}