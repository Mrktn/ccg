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

void addLabelToList(Label *label, LabelList **list)
{
    if(!*list)
    {
        *list = xmalloc(sizeof(**list));
        (*list)->label = label;
        (*list)->next = NULL;
    }

    else
    {
        LabelList *v;

        for(v = *list; v->next; v = v->next);
        v->next = xmalloc(sizeof(*v->next));
        v->next->label = label;
        v->next->next = NULL;
    }
}

Label *makeLabel(Context *context)
{
    Label *ret = xmalloc(sizeof(*ret));

    ret->name = xmalloc(12);
    sprintf(ret->name, "lbl%s", makeHexadecimalValue(8));
    context->currfunc->numlabels++;

    addLabelToList(ret, &context->currfunc->labels);

    return ret;
}

Label *selectLabel(Context *context)
{
    unsigned n = rand() % context->currfunc->numlabels, c = 0;
    LabelList *l;

    foreach(l, context->currfunc->labels)
    {
        if(l->label && c++ == n)
            return l->label;
    }

    die("unreachable !");
    return NULL;
}