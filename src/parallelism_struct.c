#include <stdlib.h>
#include <stdio.h>

#include "parallelism_struct.h"


struct substrate_parallelism_profile substrate_parallelism_profile_clone(
        struct substrate_parallelism_profile * pp)
{
    struct substrate_parallelism_profile res = {NULL,0};
    unsigned int i = 0;

    res.loop_carried_dependences = (bool*) malloc(pp->size * sizeof(bool));
    res.size = pp->size;
    for(i=0 ; i<res.size ; i++)
    {
        res.loop_carried_dependences[i] = pp->loop_carried_dependences[i];
    }

    return res;
}

void substrate_parallelism_profile_free(
        struct substrate_parallelism_profile * pp)
{
    free(pp->loop_carried_dependences);
    pp->loop_carried_dependences = NULL;
    pp->size = 0;
}

void substrate_parallelism_profile_dump(
        FILE * output_stream,
        struct substrate_parallelism_profile * pp)
{
    unsigned int i = 0;

    if(pp->size > 0)
    {
        fprintf(output_stream, "Empty loop carried dependence vector\n");
    }
    else
    {
        fprintf(output_stream, "Loop carried dependence vector : [");
        for(i=0 ; i<(pp->size-1) ; i++)
        {
            fprintf(output_stream, "%d, ", pp->loop_carried_dependences[i]);
        }
        fprintf(output_stream, "%d]\n", pp->loop_carried_dependences[pp->size-1]);
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
        max_vec = pp2->loop_carried_dependences;
    }
    else
    {
        min_size = pp2->size;
        max_size = pp1->size;
        max_vec = pp1->loop_carried_dependences;
    }

    res.loop_carried_dependences = (bool*) malloc(max_size * sizeof(bool));
    res.size = max_size;
    for(i=0 ; i<min_size ; i++)
    {
        res.loop_carried_dependences[i] = 
            pp1->loop_carried_dependences[i] || pp2->loop_carried_dependences[i];
    }
    for(i=min_size ; i<max_size ; i++)
    {
        res.loop_carried_dependences[i] = max_vec[i];
    }

    return res;
}
