#include <stdlib.h>
#include <stdio.h>

#include "substrate/vectorization_struct.h"


/**
 * @brief Make a hard copy of a substrate_vectorization_profile.
 *
 * @param[in] vp A pointer to the profile that will be cloned.
 *
 * @return A clone of the argument.
 */
struct substrate_vectorization_profile substrate_vectorization_profile_clone(
        struct substrate_vectorization_profile * vp)
{
    struct substrate_vectorization_profile res = {NULL,0};
    unsigned int i = 0;

    res.vectorizable_loops = (bool*) malloc(vp->size * sizeof(bool));
    res.size = vp->size;
    for(i=0 ; i<res.size ; i++)
    {
        res.vectorizable_loops[i] = vp->vectorizable_loops[i];
    }

    return res;
}


/**
 * @brief Free the memory taken by an substrate_vectorization_profile.
 *
 * @param[inout] vp A pointer to the vectorization profile that will
 * be freed.
 */
void substrate_vectorization_profile_free(
        struct substrate_vectorization_profile * vp)
{
    free(vp->vectorizable_loops);
    vp->vectorizable_loops = NULL;
    vp->size = 0;
}


/**
 * @brief Dump the internal data of a substrate_vectorization_profile, in a 
 * somewhat organized manner, to a FILE* (can be stdout).
 *
 * @param[in] output_stream The output FILE* used to print the dump.
 * @param[in] vp The vectorization profile that will be dumped.
 */
void substrate_vectorization_profile_dump(
        FILE * output_stream,
        struct substrate_vectorization_profile * vp)
{
    unsigned int i = 0;

    if(vp->size == 0)
    {
        fprintf(output_stream, "Empty vectorizable loop vector\n");
    }
    else
    {
        fprintf(output_stream, "Vectorizable loop vector : [");
        for(i=0 ; i<(vp->size-1) ; i++)
        {
            fprintf(output_stream, "%d, ", vp->vectorizable_loops[i]);
        }
        fprintf(output_stream, "%d]\n", vp->vectorizable_loops[vp->size-1]);
    }
}


/**
 * @brief Fusion two vectorization profiles into a third one by aggregating
 * their informations in a logical way.
 *
 * @param vp1 The first vectorization profile.
 * @param vp2 The second vectorization profile.
 *
 * @return A new substrate_vectorization_profile containing the aggregated
 * informations of the arguments.
 */
struct substrate_vectorization_profile substrate_vectorization_profile_fusion(
        struct substrate_vectorization_profile * vp1,
        struct substrate_vectorization_profile * vp2)
{
    struct substrate_vectorization_profile res = {NULL,0};
    unsigned int i = 0, min_size = 0, max_size = 0;
    bool *max_arr = NULL;

    if(vp1->size < vp2->size)
    {
        min_size = vp1->size;
        max_size = vp2->size;
        max_arr = vp2->vectorizable_loops;
    }
    else
    {
        min_size = vp2->size;
        max_size = vp1->size;
        max_arr = vp1->vectorizable_loops;
    }

    res.vectorizable_loops = (bool*) malloc(max_size * sizeof(bool));
    res.size = max_size;
    for(i=0 ; i<min_size ; i++)
    {
        res.vectorizable_loops[i] = 
            vp1->vectorizable_loops[i] && vp2->vectorizable_loops[i];
    }
    for(i=min_size ; i<max_size ; i++)
    {
        res.vectorizable_loops[i] = max_arr[i];
        //Maybe we should set it to 0 instead of max_arr[i] ?
        //res.vectorizable_loops[i] = 0;
    }

    return res;
}
