#ifndef __SUBSTRATE__PARALLELISM_STRUCT_H
#define __SUBSTRATE__PARALLELISM_STRUCT_H

#include <stdbool.h>
#include <stdio.h>

struct substrate_parallelism_profile
{
    bool * parallelizable_loops; /*< The loops that can be parallelized (from outer to inner loop) */
    unsigned int size;
};

struct substrate_parallelism_profile substrate_parallelism_profile_clone(
        struct substrate_parallelism_profile * pp);
void substrate_parallelism_profile_free(
        struct substrate_parallelism_profile * pp);
void substrate_parallelism_profile_dump(
        FILE * output_stream,
        struct substrate_parallelism_profile * pp);
void substrate_parallelism_profile_idump(
        FILE * output_stream,
        struct substrate_parallelism_profile * pp,
        unsigned int level);
struct substrate_parallelism_profile substrate_parallelism_profile_fusion(
        struct substrate_parallelism_profile * pp1,
        struct substrate_parallelism_profile * pp2);

#endif
