#ifndef __SUBSTRATE__TILING_HYPERPLANE_STRUCT_H
#define __SUBSTRATE__TILING_HYPERPLANE_STRUCT_H

#include "pluto.h"

struct substrate_tiling_hyperplane_profile
{
    int64 **vectors;

    unsigned int nb_rows;
    unsigned int nb_columns;
};

struct substrate_tiling_hyperplane_profile substrate_tiling_hyperplane_profile_malloc(
        unsigned int nb_rows,
        unsigned int nb_columns);
struct substrate_tiling_hyperplane_profile substrate_tiling_hyperplane_profile_clone(
        struct substrate_tiling_hyperplane_profile * vp);
void substrate_tiling_hyperplane_profile_free(
        struct substrate_tiling_hyperplane_profile * vp);
void substrate_tiling_hyperplane_profile_dump(
        FILE * output_stream,
        struct substrate_tiling_hyperplane_profile * vp);

struct substrate_tiling_hyperplane_profile 
    substrate_tiling_hyperplane_profile_clone_PlutoMatrix(
        PlutoMatrix * m);
#endif
