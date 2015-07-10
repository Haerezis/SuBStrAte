#include "substrate/tiling_hyperplane_struct.h"
#include "pluto.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * @brief Allocate the memory and structure needed by a 
 * substrate_tiling_hyperplane_profile, then initialize it.
 *
 * @param[in] nb_rows Number of row of the matrix contained in the profile.
 * @param[in] nb_columns Numbef of column of the matrix contained in the profile.
 *
 * @return The allocated and initialized substrate_tiling_hyperplane_profile.
 */
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


/**
 * @brief Make a hard copy of a substrate_tiling_hyperplane_profile.
 *
 * @param[in] thp A pointer to the profile that will be cloned.
 *
 * @return A clone of the argument.
 */
struct substrate_tiling_hyperplane_profile substrate_tiling_hyperplane_profile_clone(
        struct substrate_tiling_hyperplane_profile * thp)
{
    struct substrate_tiling_hyperplane_profile res = {NULL,0,0};

    res = substrate_tiling_hyperplane_profile_malloc(thp->nb_rows, thp->nb_columns);
    for(unsigned int i = 0 ; i < res.nb_rows ; i++)
    {
        for(unsigned int j = 0 ; j < res.nb_columns ; j++)
        {
            res.vectors[i][j] = thp->vectors[i][j];
        }
    }

    return res;
}


/**
 * @brief This function clone the PlutoMatrix structure,
 * representing the tiling hyperplane founded by libpluto,
 * and transform it into a substrate_tiling_hyperplane_profile.
 *
 * @param[in] m The PlutoMatrix that will be cloned.
 *
 * @return The clone of the PlutoMatrix as a substrate_tiling_hyperplane_profile.
 */
struct substrate_tiling_hyperplane_profile 
    substrate_tiling_hyperplane_profile_clone_PlutoMatrix(
        PlutoMatrix * m)
{
    struct substrate_tiling_hyperplane_profile res = {NULL, 0, 0};
    PlutoMatrix *tmp = NULL;

    tmp = pluto_matrix_alloc(m->nrows, m->ncols);
    res.vectors = tmp->val;
    res.nb_rows = tmp->nrows;
    res.nb_columns = tmp->ncols;

    for(unsigned int i = 0; i<res.nb_rows ; i++)
    {
        for(unsigned int j = 0; j<res.nb_columns ; j++)
        {
            res.vectors[i][j] = m->val[i][j];
        }
    }

    free(tmp);

    return res;
}


/**
 * @brief Free the memory taken by an substrate_tiling_hyperplane_profile.
 *
 * @param[inout] thp A pointer to the tiling hyperplane profile that will
 * be freed.
 */
void substrate_tiling_hyperplane_profile_free(
        struct substrate_tiling_hyperplane_profile * thp)
{
    for(unsigned int i = 0; i<thp->nb_rows ; i++)
    {
        free(thp->vectors[i]);
    }
    free(thp->vectors);

    thp->vectors = NULL;
    thp->nb_rows = 0;
    thp->nb_columns = 0;
}


/**
 * @brief Dump the internal data of a tiling hyperplane profile, in a 
 * somewhat organized manner, to a FILE* (can be stdout).
 *
 * @param[in] output_stream The output FILE* used to print the dump.
 * @param[in] thp The tiling hyperplane profile that will be dumped.
 */
void substrate_tiling_hyperplane_profile_dump(
        FILE * output_stream,
        struct substrate_tiling_hyperplane_profile * thp)
{
    fprintf(output_stream, "Tiling Hyperplane vectors : ");
    for(unsigned int i=0; i < thp->nb_rows; i++)
    {        
        fprintf(output_stream, "(");
        for(unsigned int j=0; j < thp->nb_columns; j++)
        {
            fprintf(output_stream, "%lld%s", thp->vectors[i][j], j<(thp->nb_columns-1)? ", ":"");
        }
        fprintf(output_stream, ")%s", i<(thp->nb_rows-1)? ", ":"");
    }
    fprintf(output_stream, "\n");
}

