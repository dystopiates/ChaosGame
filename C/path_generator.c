#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include "path_generator.h"

/*
 * The % operation doesn't do a true modulo operation, but we need that.
 */
inline uint32_t modulo(int32_t a, uint32_t b) {
    const int32_t result = a % (int32_t) b;
    return result >= 0 ? result : result + b;
}

inline size_t ipow(uint8_t base, size_t exponent) {
    size_t result = 1;

    while (exponent-- > 0) {
        result *= base;
    }

    return result;
}

size_t get_path_offset(const uint8_t *history, uint8_t history_len, uint8_t n) {
    size_t offset = 0;
    size_t heap_chunk = n + 1;

    while (history_len-- > 0) {
        offset += history[history_len] * heap_chunk;
        heap_chunk *= n + 1;
    }

    return offset;
}


#pragma clang diagnostic push
#pragma ide diagnostic ignored "misc-no-recursion"
void explore_paths_recursively(paths_t *paths, uint8_t *history, size_t depth, path_gen get_path) {
    if (depth == paths->history_len) {
        uint8_t *path_dest = follow_path(paths, history);

        get_path(path_dest, paths->n_points, history, paths->history_len);
    }
    else {
        for (uint8_t path = 0; path < paths->n_points; path++) {
            history[depth] = path;

            explore_paths_recursively(paths, history, depth + 1, get_path);
        }
    }
}
#pragma clang diagnostic pop

paths_t explore_paths(size_t history_len, uint8_t n, path_gen get_path) {
    uint8_t *history = calloc(history_len, sizeof(uint8_t));

    paths_t paths = {
        .history_len = history_len,
        .n_points = n,
        .path = calloc(ipow(n + 1, history_len + 1), sizeof(uint8_t)),
        .heap_chunk_sizes = calloc(history_len, sizeof(size_t))
    };

    for (size_t i = 1; i <= history_len; i++) {
        paths.heap_chunk_sizes[i - 1] = ipow(n + 1, i);
    }

    explore_paths_recursively(&paths, history, 0, get_path);

    free(history);

    return paths;
}


void free_paths_t(paths_t *paths) {
//    free_paths_recursively(paths->path, paths->history_len, paths->n_points);
    free(paths->path);
    paths->path = NULL;
    free(paths->heap_chunk_sizes);
    paths->heap_chunk_sizes = NULL;
}

uint8_t *follow_path(paths_t *path, const uint8_t *history) {
    /*
     * I thought keeping the path in a heap rather than in a 2D list would help with
     * CPU caching, since everything would be kept contiguous, but it seems I've been
     * outsmarted by the GCC optimizer once more, and it makes no difference. The heap
     * logic stayed behind anyway.
     */
    return path->path + get_path_offset(history, path->history_len, path->n_points);

//    void **followed_path = path->path;
//
//    for (size_t depth = 0; depth < path->history_len; depth++) {
//        followed_path = (void **) followed_path[history[depth]];
//    }
//
//    return (uint8_t *) followed_path;
}


void fancy(uint8_t *dest, uint8_t n, const uint8_t *history, size_t history_len) {
    if (history_len < 2) {
        fputs("History length not enough for fancy path generator\n", stderr);
        abort();
    }

    if (history[history_len - 1] == history[history_len - 2]) {
        uint8_t p = history[history_len - 1];

        uint8_t lower = modulo((int32_t) p - 1, n);
        uint8_t upper = modulo((int32_t) p + 1, n);

        size_t i;
        uint8_t path;
        for (i = 0, path = 0; path < n; path++) {
            if (path != lower && path != upper) {
                dest[i++] = path;
            }
        }
        dest[i] = ~0;
    }
    else {
        size_t i;
        for (i = 0; i < n; i++) {
            dest[i] = (uint8_t) i;
        }
        dest[i] = ~0;
    }
}


void no_neighbors(uint8_t *dest, uint8_t n, const uint8_t *history, size_t history_len) {
    if (history_len < 1) {
        fputs("History length not enough for fancy path generator\n", stderr);
        abort();
    }

    /*
     * Haven't gotten around to implementing this in C yet.
     */
}
