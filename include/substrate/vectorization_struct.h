#ifndef __SUBSTRATE__VECTORIZATION_STRUCT_H
#define __SUBSTRATE__VECTORIZATION_STRUCT_H

#include <stdbool.h>
#include <stdio.h>

struct substrate_vectorization_profile
{
    bool * vectorizable_loops; /*< The loops that can be vectorized (from outer to inner loop) */
    unsigned int size; /*< The size of the vectorizable_loops array */
};

struct substrate_vectorization_profile substrate_vectorization_profile_clone(
        struct substrate_vectorization_profile * vp);
void substrate_vectorization_profile_free(
        struct substrate_vectorization_profile * vp);
void substrate_vectorization_profile_dump(
        FILE * output_stream,
        struct substrate_vectorization_profile * vp);
void substrate_vectorization_profile_idump(
        FILE * output_stream,
        struct substrate_vectorization_profile * vp,
        unsigned int level);
struct substrate_vectorization_profile substrate_vectorization_profile_fusion(
        struct substrate_vectorization_profile * vp1,
        struct substrate_vectorization_profile * vp2);

#endif
