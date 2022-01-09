#include <malloc.h>
#include <math.h>
#include <stdlib.h>
#include "render.h"


void fill_corners(point_t *corners, uint8_t n_points) {
    double tau = 2 * M_PI;

    double radians = tau / n_points;

    for (uint8_t i = 0; i < n_points; i++) {
        corners[i].x = cos(i * radians);
        corners[i].y = sin(i * radians);
    }
}

void find_best_scale(point_t *corners, uint8_t n_points, uint32_t size, double *scale, double *dx, double *dy) {
    double min_x = 0, max_x = 0;
    double min_y = 0, max_y = 0;

    for (uint8_t i = 0; i < n_points; i++) {
        min_x = fmin(min_x, corners[i].x);
        max_x = fmax(max_x, corners[i].x);
        min_y = fmin(min_y, corners[i].y);
        max_y = fmax(max_y, corners[i].y);
    }

    double range_x = max_x - min_x;
    double range_y = max_y - min_y;

    *scale = fmin(size / range_x, size / range_y);
    *dx = -*scale * ((max_x + min_x) / 2.0) + (size / 2.0);
    *dy = -*scale * ((max_y + min_y) / 2.0) + (size / 2.0);
}

renderer_t prepare_renderer(uint8_t n_points, uint8_t depth, uint32_t size) {
    renderer_t renderer = {
        .n_points = n_points,
        .depth = depth + 1,
        .size = size,
        .counts = calloc(size * size, sizeof(uint32_t)),
        .indices = malloc(size * sizeof(uint32_t *)),
        .corners = malloc(n_points * sizeof(point_t)),
        .current_path = malloc((depth + 1) * sizeof(point_t)),
        .current_depth = 1
    };

    for (uint32_t i = 0; i < size; i++)
        renderer.indices[i] = renderer.counts + i * size;
    fill_corners(renderer.corners, n_points);

    renderer.current_path[0].x = 0;
    renderer.current_path[0].y = 0;

    double scale, dx, dy;
    find_best_scale(renderer.corners, n_points, size, &scale, &dx, &dy);

    for (uint8_t i = 0; i < n_points; i++) {
        renderer.corners[i].x = renderer.corners[i].x * scale + dx;
        renderer.corners[i].y = renderer.corners[i].y * scale + dy;
    }

    return renderer;
}

void dfs_count(renderer_t *renderer, uint8_t corner) {
    if (corner == 0xff) {
        dfs_pop(renderer);
        return;
    }
    else if (corner >= renderer->n_points) {
        fputs("Found unknown corner, aborting\n", stderr);
        abort();
    }

    point_t point = average(renderer->current_path[renderer->current_depth - 1], renderer->corners[corner]);
    renderer->indices[(int) point.x][(int) point.y]++;

    if (renderer->current_depth < renderer->depth)
        dfs_push(renderer, point);
}

void dfs_push(renderer_t *renderer, point_t point) {
    if (renderer->current_depth == renderer->depth) {
        fputs("Cannot push renderer past its max depth\n", stderr);
        abort();
    }

    renderer->current_path[renderer->current_depth] = point;

    renderer->current_depth++;
}

void dfs_pop(renderer_t *renderer) {
    if (renderer->current_depth == 0xff) {
        fputs("Cannot pop renderer, depth is 0\n", stderr);
        abort();
    }

    renderer->current_depth--;
}

void free_renderer(renderer_t *renderer) {
    free(renderer->counts);
    free(renderer->indices);
    free(renderer->corners);
    free(renderer->current_path);
}
