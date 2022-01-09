#ifndef CHAOSGENERATOR_CHAOS_ITERATOR_H
#define CHAOSGENERATOR_CHAOS_ITERATOR_H

#include <stdint.h>
#include <stdio.h>
#include "path_generator.h"

uint64_t iterate(paths_t *paths, uint8_t depth, FILE *fp);

#endif //CHAOSGENERATOR_CHAOS_ITERATOR_H
