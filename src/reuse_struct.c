#include <stdlib.h>

#include "reuse_struct.h"

void substrate_reuse_profile_free(
        struct substrate_reuse_profile * rp)
{
    unsigned int i = 0;
    for(i=0; i < rp->size ; i++)
        substrate_array_profile_free(&rp->array_profiles[i]);
    free(rp->array_profiles);
    rp->size = 0;
}

void substrate_array_profile_free(
        struct substrate_array_profile * ap)
{
    unsigned int i = 0;
    for(i=0 ; i < ap->size ; i++)
        substrate_uniformly_generated_set_free(&ap->uniformly_gen_sets[i]);
    free(ap->uniformly_gen_sets);
    ap->size = 0;
}
void substrate_uniformly_generated_set_free(
        struct substrate_uniformly_generated_set * ugs)
{
    unsigned int i = 0;
    for(i=0 ; i< ugs->size ; i++)
        substrate_equivalence_class_free(&ugs->classes[i]);
    free(ugs->classes);
    ugs->size = 0;
}

void substrate_equivalence_class_free(
        struct substrate_equivalence_class * ec)
{
    /*
    substrate_reuse_space_free(&ec->reuse_space);
    free(ec->array_references);
    */
    ec->size = 0;
}

void substrate_reuse_space_free(
        struct substrate_reuse_space * rp)
{
    free(rp->spanning_vector_indexes);
    rp->size = 0;

}



void substrate_reuse_profile_dump(
        FILE * output_stream,
        struct substrate_reuse_profile * rp)
{
    unsigned int i = 0;

    fprintf(output_stream,"Number of array in the statement : %d\n", rp->size);
    for(i=0 ; i<rp->size ; i++)
    {
        substrate_array_profile_dump(output_stream, &rp->array_profiles[i]);
    }
}


void substrate_array_profile_dump(
        FILE * output_stream,
        struct substrate_array_profile * ap)
{
    unsigned int i = 0;

    fprintf(output_stream,"\tArray id : ");
    osl_int_print(output_stream,ap->uniformly_gen_sets[0].H_matrix->precision, ap->array_id);
    fprintf(output_stream,"\n");
    fprintf(output_stream,"\tNumber of references : %d\n",ap->size);
    for(i=0; i<ap->size ; i++)
    {
        substrate_uniformly_generated_set_dump(output_stream,&ap->uniformly_gen_sets[i]);
    }
}


void substrate_uniformly_generated_set_dump(
        FILE * output_stream,
        struct substrate_uniformly_generated_set * ugs)
{
    unsigned int i = 0;

    fprintf(output_stream,"\t\tH matrix :\n");
    osl_relation_idump(output_stream,ugs->H_matrix,2);
    fprintf(output_stream,"\t\tNumber of equivalence classes : %d\n",ugs->size);
    fprintf(output_stream,"\t\tEquivalence classes\n");
    for(i=0; i<ugs->size ; i++)
    {
        substrate_equivalence_class_dump(output_stream,&ugs->classes[i]);
    }
}


void substrate_equivalence_class_dump(
        FILE * output_stream,
        struct substrate_equivalence_class * ec)
{
    fprintf(output_stream,"\n");
    fprintf(output_stream,"\t\t######################################################\n");
    fprintf(output_stream,"\n");
    osl_relation_list_idump(output_stream,ec->array_references,3);
}
