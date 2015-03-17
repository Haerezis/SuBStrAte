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
            //statement->access,
            osl_relation_list_clone(statement->access),
            substrate_array_id_eq);

    //We will create a profile for each array, and there will be as many array
    //profile as there is access relation group, so we use relations_group_list 
    //size for array_profiles size
    reuse_profile.size = relations_group_list.size;
    reuse_profile.array_profiles = (struct substrate_array_profile*)
        malloc(relations_group_list.size * sizeof(struct substrate_array_profile));

    //We create an array profile for each array/access relation group.
    for(i=0 ; i<relations_group_list.size ; i++)
    {
        reuse_profile.array_profiles[i] = substrate_array_profile_constructor(
                relations_group_list.list[i]);
    }

    free(relations_group_list.list);

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
            array_access_rel_list,
            //osl_relation_list_clone(array_access_rel_list),
            substrate_H_matrix_eq);

    //All the array id in array_access_rel_list should be the same so we get 1st.
    //There will be as much substrate_uniformly_generated_set in array_profile as
    //there was groups/uniformly generated set in uniformly_generated_set_list :
    //so we use its size for the allocation.
    array_profile.array_id_precision = array_access_rel_list->elt->precision;
    osl_int_init_set(
            array_profile.array_id_precision,
            &array_profile.array_id,
            substrate_get_array_id_from_access_relation(array_access_rel_list->elt));
    array_profile.size = uniformly_generated_set_list.size;
    array_profile.uniformly_gen_sets = (struct substrate_uniformly_generated_set*)
        malloc(array_profile.size * sizeof(struct substrate_uniformly_generated_set));

    for(i=0 ; i< array_profile.size ; i++)
    {
        array_profile.uniformly_gen_sets[i] = 
            substrate_uniformly_gen_set_constructor(uniformly_generated_set_list.list[i]);
    }
    
    free(uniformly_generated_set_list.list);

    return array_profile;
}

struct substrate_uniformly_generated_set substrate_uniformly_gen_set_constructor(
        struct osl_relation_list * uniformly_generated_set)
{
    struct substrate_uniformly_generated_set uni_gen_set = {NULL,NULL,0};
    struct substrate_osl_relation_group_list classes;
    unsigned int i = 0;

    classes = substrate_group_access_relations_by(
            uniformly_generated_set,
            //osl_relation_list_clone(uniformly_generated_set),
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

    free(classes.list);

    return uni_gen_set;
}


struct substrate_equivalence_class substrate_equivalence_class_constructor(
        struct osl_relation_list * array_references)
{
    struct substrate_equivalence_class eq_class = {NULL,0};

    eq_class.array_references = array_references;
    while(array_references != NULL)
    {
        eq_class.size++;
        array_references = array_references->next;
    }

    return eq_class;
}



double substrate_rate_reuse_profiles(
        struct substrate_reuse_profile rp1,
        struct substrate_reuse_profile rp2)
{
    double rating = 0;
    unsigned int i1 = 0, i2 = 0;

    for(i1=0; i1<rp1.size ; i1++)
    {
        for(i2=0 ; i2<rp2.size ; i2++)
        {
            if(osl_int_eq(
                        rp1.array_profiles[i1].array_id_precision,
                        rp1.array_profiles[i1].array_id,
                        rp2.array_profiles[i2].array_id))
            {
                //TODO rating
                rating += substrate_rate_array_profiles(
                        rp1.array_profiles[i1],
                        rp2.array_profiles[i2]);
            }
        }
    }

    return rating;
}

double substrate_rate_array_profiles(
        struct substrate_array_profile ap1,
        struct substrate_array_profile ap2)
{
    double rating = 0;
    unsigned int i1 = 0, i2 = 0;

    for(i1=0; i1<ap1.size ; i1++)
    {
        for(i2=0 ; i2<ap2.size ; i2++)
        {
            if(substrate_H_matrix_eq(
                        ap1.uniformly_gen_sets[i1].H_matrix,
                        ap2.uniformly_gen_sets[i2].H_matrix))
            {
                //rating TODO
                rating += substrate_rate_uniformly_generated_sets(
                        ap1.uniformly_gen_sets[i1],
                        ap2.uniformly_gen_sets[i2]);
            }
        }
    }

    return rating;
}

double substrate_rate_uniformly_generated_sets(
        struct substrate_uniformly_generated_set ugs1,
        struct substrate_uniformly_generated_set ugs2)
{
    double rating = 0;
    bool has_hit = true;
    unsigned int nb_hit = 0;
    unsigned int nb_miss = 0;
    unsigned int i1 = 0, i2 = 0;

    for(i1=0; i1<ugs1.size ; i1++)
    {
        has_hit = false;
        for(i2=0 ; (i2<ugs2.size) && (has_hit == false) ; i2++)
        {
            has_hit = substrate_access_class_eq(
                            ugs1.classes[i1].array_references->elt,
                            ugs2.classes[i2].array_references->elt);
        }
        if(has_hit)
            nb_hit++;
        else
            nb_miss++;
    }

    //TODO rating
    rating = nb_hit/nb_miss;
    return rating;
}
