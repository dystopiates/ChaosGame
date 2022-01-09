#include "point.h"

point_t average(point_t a, point_t b) {
    point_t point = {
        .x = (a.x + b.x) * 0.5,
        .y = (a.y + b.y) * 0.5
    };

    return point;
}
