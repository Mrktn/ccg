#include "ccg.h"

void die(const char *format, ...)
{
    va_list vargs;

    va_start(vargs, format);
    fprintf(stderr, "ccg: ");
    vfprintf(stderr, format, vargs);
    fprintf(stderr, "\n");
    exit(EXIT_FAILURE);
}

void info(const char *format, ...)
{
    va_list vargs;

    va_start(vargs, format);
    fprintf(stdout, "(\033[32mI\033[0m) ccg: ");
    vfprintf(stdout, format, vargs);
    fprintf(stdout, "\n");
}

void *xmalloc(size_t size)
{
    void *ptr = malloc(size);

    /* Since xmalloc is a hot path, let's use branch prediction to optimize it a bit */
    if(unlikely(!ptr))
        die("cannot allocate memory !");

    return ptr;
}

void *xcalloc(size_t nmemb, size_t size)
{
    void *ptr = calloc(nmemb, size);

    if(unlikely(!ptr))
        die("cannot allocate memory !");

    return ptr;
}
