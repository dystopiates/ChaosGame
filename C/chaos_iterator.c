#include <malloc.h>
#include "chaos_iterator.h"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "misc-no-recursion"
uint64_t iterate_recursively(
    paths_t *paths, uint8_t *history, uint8_t depth, FILE *fp
) {
    uint8_t *option = follow_path(paths, history - paths->history_len);

    if (depth == 0) {
        size_t option_len = 0;
        while (option[option_len] != 0xff)
            option_len++;

        fwrite(option, 1, option_len + 1, fp);

        return (uint64_t) option_len;
    }
    else {
        uint64_t points_visited = 0;

        for ( ; *option != 0xff; option++) {
            fwrite(option, 1, 1, fp);
            points_visited += 1;

            *history = *option;
            points_visited += iterate_recursively(paths, history + 1, depth - 1, fp);
        }

        fwrite(option, 1, 1, fp);

        return points_visited;
    }
}
#pragma clang diagnostic pop

uint64_t iterate(paths_t *paths, uint8_t depth, FILE *fp) {
    fwrite(&paths->n_points, 1, 1, fp);
    fwrite(&depth, 1, 1, fp);

    uint8_t *history = calloc(depth + paths->history_len, sizeof(uint32_t));

    uint64_t visited = iterate_recursively(paths, history + paths->history_len, depth, fp);

    free(history);

    return visited;
}
