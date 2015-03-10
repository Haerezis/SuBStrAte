#include <stdlib.h>

#include "osl/osl.h"
#include "osl/int.h"

#include "reuse.h"
#include "utils.h"

struct substrate_reuse_profile substrate_generate_reuse_profile(
       struct osl_statement * statement)
{
    struct substrate_reuse_profile reuse_profile;
    struct substrate_osl_int_set array_id_set = {NULL,0,0};
    struct osl_relation_list *current_relation = NULL;
    unsigned int i = 0;

    array_id_set = substrate_array_id_set_from_access_relations(statement->access);

    
    reuse_profile.osl_statement = statement;
    reuse_profile.size = array_id_set.size;
    reuse_profile.array_profiles = (struct substrate_array_profile*)
        malloc(array_id_set.size * sizeof(struct substrate_array_profile));

    for(i=0 ; i<array_id_set.size ; i++)
    {
        reuse_profile.array_profiles[i] = substrate_generate_array_profile(
                statement,
                array_id_set.set[i]);
    }
    
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
