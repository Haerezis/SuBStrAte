#include <stdlib.h>

#include "osl/osl.h"
#include "osl/int.h"

#include "reuse.h"
#include "utils.h"

struct substrate_reuse_profile substrate_reuse_profile_constructor(
       struct osl_statement * statement)
{
    struct substrate_reuse_profile reuse_profile;
    struct substrate_osl_relation_group_list relations_group_list = {NULL,0,0};
    unsigned int i = 0;

    //We class the different access relation according to the array of the access
    //relation : each array reference of the statement should have a group.
    relations_group_list = substrate_group_access_relations_by(
            osl_relation_list_clone(statement->access),
            substrate_array_id_eq);

    //We will create a profile for each array, and there will be as many array
    //profile as there is access relation group, so we use relations_group_list 
    //size for array_profiles size
    reuse_profile.osl_statement = statement;
    reuse_profile.size = relations_group_list.size;
    reuse_profile.array_profiles = (struct substrate_array_profile*)
        malloc(relations_group_list.size * sizeof(struct substrate_array_profile));

    //We create an array profile for each array/access relation group.
    for(i=0 ; i<relations_group_list.size ; i++)
    {
        reuse_profile.array_profiles[i] = substrate_array_profile_constructor(
                relations_group_list.list[i]);
    }

    return reuse_profile;
}

struct substrate_array_profile substrate_array_profile_constructor(
        struct osl_relation_list * array_access_rel_list)
{
    struct substrate_array_profile array_profile;
    struct substrate_osl_relation_group_list uniformly_generated_set_list;
    unsigned int i = 0;

    //We group all the access relation by their H matrix.
    //Each group correspond to a Uniformly Generated Set.
    uniformly_generated_set_list = substrate_group_access_relations_by(
            osl_relation_list_clone(array_access_rel_list),
            substrate_H_matrix_eq);

    //All the array id in array_access_rel_list should be the same so we get 1st.
    //There will be as much substrate_uniformly_generated_set in array_profile as
    //there was groups/uniformly generated set in uniformly_generated_set_list :
    //so we use its size for the allocation.
    array_profile.array_id = substrate_get_array_id_from_access_relation(
            array_access_rel_list->elt);
    array_profile.size = uniformly_generated_set_list.size;
    array_profile.uniformly_gen_sets = (struct substrate_uniformly_generated_set*)
        malloc(array_profile.size * sizeof(struct substrate_uniformly_generated_set));

    for(i=0 ; i< array_profile.size ; i++)
    {
        array_profile.uniformly_gen_sets[i] = 
            substrate_uniformly_gen_set_constructor(uniformly_generated_set_list.list[i]);
    }

    return array_profile;
}

struct substrate_uniformly_generated_set substrate_uniformly_gen_set_constructor(
        struct osl_relation_list * uniformly_generated_set)
{
    struct substrate_uniformly_generated_set uni_gen_set = {NULL,NULL,0};
    struct substrate_osl_relation_group_list classes;
    unsigned int i = 0;

    classes = substrate_group_access_relations_by(
            osl_relation_list_clone(uniformly_generated_set),
            substrate_access_class_eq);
    
    uni_gen_set.H_matrix = uniformly_generated_set->elt;
    uni_gen_set.classes = (struct substrate_equivalence_class*)
        malloc(classes.size * sizeof(struct substrate_equivalence_class));
    uni_gen_set.size = classes.size;

    for(i=0; i<uni_gen_set.size ; i++)
    {
        uni_gen_set.classes[i] = 
            substrate_equivalence_class_constructor(classes.list[i]);
    }

    return uni_gen_set;
}


struct substrate_equivalence_class substrate_equivalence_class_constructor(
        struct osl_relation_list * array_references)
{
    struct substrate_equivalence_class eq_class = {NULL,0};

    eq_class.array_references = array_references;
    while(array_references != NULL) eq_class.size++;

    return eq_class;
}
