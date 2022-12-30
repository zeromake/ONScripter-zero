#include <stdio.h>
#include <stdarg.h>

void va_func(char* fmt, ...) {
    va_list al;
    va_start(al, fmt);
    int s = va_arg(al, int);
    va_end(al);
    printf(fmt, s);
}


// typedef struct {
//     void *__stack;
//     void *__gr_top;
//     void *__vr_top;
//     int   __gr_offs;
//     int   __vr_offs;
// } builtin_va_list;

int main() {
    // union va_list_wrap {
    //     builtin_va_list pa;
    //     va_list al;
    // } al;
    // int a = 1000;
    // intptr_t *stack = (intptr_t *)malloc(sizeof(intptr_t));
    // *stack = a;
    // al.pa.__stack = stack;
    // al.pa.__vr_offs = 0;
    // al.pa.__gr_offs = 0;
    // vprintf("%d\n", al.al);
    va_func("%d\n", 5);
}