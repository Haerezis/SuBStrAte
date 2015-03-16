#include <stdlib.h>

#include "reuse_struct.h"

void substrate_reuse_profile_free(
        struct substrate_reuse_profile * rp)
{
    unsigned int i = 0;
    for(i=0; i < rp->size ; i++)
        substrate_array_profile_free(&rp->array_profiles[i]);
    free(rp->array_profiles);
    rp->array_profiles = NULL;
    rp->size = 0;
}

void substrate_array_profile_free(
        struct substrate_array_profile * ap)
{
    unsigned int i = 0;
    for(i=0 ; i < ap->size ; i++)
        substrate_uniformly_generated_set_free(&ap->uniformly_gen_sets[i]);
    free(ap->uniformly_gen_sets);
    ap->uniformly_gen_sets = NULL;
    ap->size = 0;

    osl_int_clear(ap->array_id_precision, &ap->array_id);
    ap->array_id_precision = 0;
}
void substrate_uniformly_generated_set_free(
        struct substrate_uniformly_generated_set * ugs)
{
    unsigned int i = 0;
    for(i=0 ; i< ugs->size ; i++)
        substrate_equivalence_class_free(&ugs->classes[i]);
    free(ugs->classes);
    ugs->classes = NULL;
    ugs->size = 0;
    
    ugs->H_matrix = NULL;
}

void substrate_equivalence_class_free(
        struct substrate_equivalence_class * ec)
{
    osl_relation_list_free(ec->array_references);
    ec->array_references = NULL;
    ec->size = 0;
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
    osl_int_print(output_stream,ap->array_id_precision, ap->array_id);
    fprintf(output_stream,"\n");
    fprintf(output_stream,"\tNumber of different Uniformly Generated Sets : %d\n",ap->size);
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
    fprintf(output_stream,"\t\tNumber of references in the class : %d\n",ec->size);
    fprintf(output_stream,"\n");
    osl_relation_list_idump(output_stream,ec->array_references,3);
}
