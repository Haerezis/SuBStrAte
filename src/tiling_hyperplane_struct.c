#include "substrate/tiling_hyperplane_struct.h"
#include "pluto.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

struct substrate_tiling_hyperplane_profile substrate_tiling_hyperplane_profile_malloc(
        unsigned int nb_rows,
        unsigned int nb_columns)
{
    struct substrate_tiling_hyperplane_profile res = {NULL,0,0};
    unsigned int i = 0;

    res.nb_rows = nb_rows;
    res.nb_columns = nb_columns;
    res.vectors = (int64**) malloc(sizeof(int64*) * res.nb_rows);

    for(i=0 ; i<res.nb_rows ; i++)
    {
        res.vectors[i] = (int64*) malloc(sizeof(int64) * res.nb_columns);
        memset(res.vectors[i],'0',sizeof(int64)*res.nb_columns);
    }

    return res;
}

struct substrate_tiling_hyperplane_profile substrate_tiling_hyperplane_profile_clone(
        struct substrate_tiling_hyperplane_profile * vp)
{
    struct substrate_tiling_hyperplane_profile res = {NULL,0,0};

    res = substrate_tiling_hyperplane_profile_malloc(vp->nb_rows, vp->nb_columns);
    for(unsigned int i = 0 ; i < res.nb_rows ; i++)
    {
        for(unsigned int j = 0 ; j < res.nb_columns ; j++)
        {
            res.vectors[i][j] = vp->vectors[i][j];
        }
    }

    return res;
}

struct substrate_tiling_hyperplane_profile 
    substrate_tiling_hyperplane_profile_clone_PlutoMatrix(
        PlutoMatrix * m)
{
    struct substrate_tiling_hyperplane_profile res = {NULL, 0, 0};
    PlutoMatrix *tmp = NULL;

    tmp = pluto_matrix_dup(m);
    res.vectors = tmp->val;
    res.nb_rows = tmp->nrows;
    res.nb_columns = tmp->ncols;

    free(tmp);

    return res;
}

void substrate_tiling_hyperplane_profile_free(
        struct substrate_tiling_hyperplane_profile * vp)
{
    for(unsigned int i = 0; i<vp->nb_rows ; i++)
    {
        free(vp->vectors[i]);
    }
    free(vp->vectors);

    vp->vectors = NULL;
    vp->nb_rows = 0;
    vp->nb_columns = 0;
}

void substrate_tiling_hyperplane_profile_dump(
        FILE * output_stream,
        struct substrate_tiling_hyperplane_profile * vp)
{
    fprintf(output_stream, "Tiling Hyperplane vectors : ");
    for(unsigned int i=0; i < vp->nb_rows; i++)
    {        
        fprintf(output_stream, "(");
        for(unsigned int j=0; j < vp->nb_columns; j++)
        {
            fprintf(output_stream, "%lld%s", vp->vectors[i][j], j<(vp->nb_columns-1)? ", ":"");
        }
        fprintf(output_stream, ")%s", i<(vp->nb_rows-1)? ", ":"");
    }
    fprintf(output_stream, "\n");
}

