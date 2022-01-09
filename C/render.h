#ifndef CHAOSGENERATOR_RENDER_H
#define CHAOSGENERATOR_RENDER_H

#include <stdint.h>
#include "point.h"


typedef struct {
    uint8_t n_points;
    uint8_t depth;

    uint32_t size;

    uint32_t *counts;
    uint32_t **indices;

    point_t *corners;

    point_t *current_path;
    uint8_t current_depth;
} renderer_t;


renderer_t prepare_renderer(uint8_t n_points, uint8_t depth, uint32_t size);
void dfs_count(renderer_t *renderer, uint8_t corner);
void dfs_push(renderer_t *renderer, point_t point);
void dfs_pop(renderer_t *renderer);
void free_renderer(renderer_t *renderer);

#endif //CHAOSGENERATOR_RENDER_H
