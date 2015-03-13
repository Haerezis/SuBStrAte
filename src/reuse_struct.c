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
