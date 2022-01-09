#ifndef CHAOSGENERATOR_MAIN_HELPER_H
#define CHAOSGENERATOR_MAIN_HELPER_H

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include "chaos_iterator.h"

typedef enum { Chart, Render } ProgramMode;


inline size_t imin(size_t a, size_t b) {
    if (a < b)
        return a;
    else
        return b;
}

inline size_t imax(size_t a, size_t b) {
    if (a > b)
        return a;
    else
        return b;
}


const char *commafy(uint64_t n) {
    // 18446744073709551616
    static char num_only[21];
    // 18,446,744,073,709,551,616
    static char str[27];

    num_only[0] = 0;

    uint32_t digits = sprintf(num_only, "%ld", n);
    uint32_t until_comma = digits % 3;
    if (until_comma == 0) until_comma = 3;

    uint32_t r, w;
    for (r = 0, w = 0; r < digits; r++, until_comma--) {
        if (until_comma == 0) {
            str[w++] = ',';
            until_comma = 3;
        }

        str[w++] = num_only[r];
    }

    str[w] = 0;

    return str;
}


#endif //CHAOSGENERATOR_MAIN_HELPER_H
