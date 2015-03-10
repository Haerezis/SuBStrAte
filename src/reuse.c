#include "reuse.h"
#include "osl/int.h"

struct substrate_reuse_profile substrate_generate_reuse_profile(
       struct osl_statement * statement)
{
    struct substrate_reuse_profile reuse_profile;
    struct osl_relation_list *current_relation = NULL;
    unsigned int i = 0;

    for(i=0; current_relation != NULL ; i++)
        current_relation=current_relation->next;

    /*
    reuse_profile.osl_statement = statement;
    reuse_profile.size = 0;
    reuse_profile.array_profiles = (struct substrate_array_profile*)
        malloc(i * sizeof(struct substrate_array_profile));
    */
    return reuse_profile;
}

struct substrate_array_profile substrate_generate_array_profile(
        struct osl_statement * statement,
        osl_int_t array_index)
{
    struct substrate_array_profile tmp;
    return tmp;
}

struct substrate_uniformly_generated_set substrate_generate_uniformly_gen_set(
        struct osl_statement * statement,
        osl_int_t array_index,
        struct osl_relation * H_matrix)
{
    struct substrate_uniformly_generated_set tmp;
    return tmp;
}

struct substrate_equivalence_class substrate_generate_reuse_space(
        struct substrate_equivalence_class * ec)
{
    struct substrate_equivalence_class tmp;
    return tmp;
}
