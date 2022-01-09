#ifndef CHAOSGENERATOR_PATH_GENERATOR_H
#define CHAOSGENERATOR_PATH_GENERATOR_H

#include <stdint.h>
#include <stddef.h>


typedef struct {
    size_t history_len;
    uint8_t n_points;
    void *path;
    size_t *heap_chunk_sizes;
} paths_t;


typedef void (*path_gen)(uint8_t *dest, uint8_t n, const uint8_t *history, size_t history_len);

paths_t explore_paths(size_t history_len, uint8_t n, path_gen get_path);
uint8_t *follow_path(paths_t *path, const uint8_t *history);
void free_paths_t(paths_t *paths);

void fancy(uint8_t *dest, uint8_t n, const uint8_t *history, size_t history_len);
void no_neighbors(uint8_t *dest, uint8_t n, const uint8_t *history, size_t history_len);

#endif //CHAOSGENERATOR_PATH_GENERATOR_H
