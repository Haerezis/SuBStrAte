#include <stdlib.h>
#include <stdio.h>

#include "parallelism_struct.h"


struct substrate_parallelism_profile substrate_parallelism_profile_clone(
        struct substrate_parallelism_profile * pp)
{
    struct substrate_parallelism_profile res = {NULL,0};
    unsigned int i = 0;

    res.parallelizable_loops = (bool*) malloc(pp->size * sizeof(bool));
    res.size = pp->size;
    for(i=0 ; i<res.size ; i++)
    {
        res.parallelizable_loops[i] = pp->parallelizable_loops[i];
    }

    return res;
}

void substrate_parallelism_profile_free(
        struct substrate_parallelism_profile * pp)
{
    free(pp->parallelizable_loops);
    pp->parallelizable_loops = NULL;
    pp->size = 0;
}

void substrate_parallelism_profile_dump(
        FILE * output_stream,
        struct substrate_parallelism_profile * pp)
{
    unsigned int i = 0;

    if(pp->size == 0)
    {
        fprintf(output_stream, "Empty parallelizable loop vector\n");
    }
    else
    {
        fprintf(output_stream, "Parallelizable loop vector : [");
        for(i=0 ; i<(pp->size-1) ; i++)
        {
            fprintf(output_stream, "%d, ", pp->parallelizable_loops[i]);
        }
        fprintf(output_stream, "%d]\n", pp->parallelizable_loops[pp->size-1]);
    }
}

struct substrate_parallelism_profile substrate_parallelism_profile_fusion(
        struct substrate_parallelism_profile * pp1,
        struct substrate_parallelism_profile * pp2)
{
    struct substrate_parallelism_profile res = {NULL,0};
    unsigned int i = 0, min_size = 0, max_size = 0;
    bool *max_vec = NULL;

    if(pp1->size < pp2->size)
    {
        min_size = pp1->size;
        max_size = pp2->size;
        max_vec = pp2->parallelizable_loops;
    }
    else
    {
        min_size = pp2->size;
        max_size = pp1->size;
        max_vec = pp1->parallelizable_loops;
    }

    res.parallelizable_loops = (bool*) malloc(max_size * sizeof(bool));
    res.size = max_size;
    for(i=0 ; i<min_size ; i++)
    {
        res.parallelizable_loops[i] = 
            pp1->parallelizable_loops[i] && pp2->parallelizable_loops[i];
    }
    for(i=min_size ; i<max_size ; i++)
    {
        res.parallelizable_loops[i] = max_vec[i];
        //XXX Maybe we need to set to 0 instead of max_vec[i] ?
        //res.parallelizable_loops[i] = 0;
    }

    return res;
}
