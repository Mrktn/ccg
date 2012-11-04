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

#ifdef DEBUG_MEMORY
unsigned long long int allocated_bytes = 0;
#endif

void die(const char *format, ...)
{
    va_list vargs;

    va_start(vargs, format);
    fprintf(stderr, "ccg: ");
    vfprintf(stderr, format, vargs);
    fprintf(stderr, "\n");
    exit(EXIT_FAILURE);
}

void *xmalloc(size_t size)
{
    void *ptr = malloc(size);

#ifdef DEBUG_MEMORY
    allocated_bytes += size;
#endif

    if(!ptr)
        die("cannot allocate memory !");

    return ptr;
}

void *xcalloc(size_t nmemb, size_t size)
{
    void *ptr = calloc(nmemb, size);

#ifdef DEBUG_MEMORY
    allocated_bytes += nmemb * size;
#endif

    if(!ptr)
        die("cannot allocate memory !");

    return ptr;
}

/* Generate n stars */
char *genStars(size_t n)
{
    static char stars[16];

    memset(stars, '*', n);
    stars[n] = 0;

    return stars;
}