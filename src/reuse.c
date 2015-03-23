#include <stdlib.h>

#include "osl/osl.h"
#include "osl/int.h"

#include "reuse.h"
#include "utils.h"

/**
 * @brief Create a reuse profile by analyzing an osl_statement.
 *
 * The reuse profile is created the following way :
 * - We group all references of a same array/variable in a group (done in this function).
 * - For each group, we group the references having the same H matrix
 *   (→Uniformly Generated Sets , Wolf91).
 * - For each Uniformly Generated Sets, we group again the references according to their
 *   equivalence class (Wolf91).
 * These structure represent the reuse profile of the statement.
 *
 * @param[in] statement The osl_statement which will be analyzed to create the reuse profile.
 *
 * @return A reuse profile for the statement represented by \a statement .
 */
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



/**
 * @brief Create an array profile from a list of access relations, by grouping them per
 * Uniformly Generated Sets. After a call to this function, the relations list
 * should not be accessed anymore (nor freed) because it has been modified.
 *
 * @param[in] array_access_rel_list A list of access relations representing all the
 * access references of a same array in a statement.
 *
 * @return An array profile.
 */
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

/**
 * @brief Create a Uniformly Generated Set profile, grouping access relation in their
 * respective equivalence class. After a call to this function, the relations list
 * should not be accessed anymore (nor freed) because it has been modified.
 *
 * @param[in] uniformly_generated_set A list of access relation belonging to the same
 * Uniformly Generated Set.
 *
 * @return A Uniformly Generated Set profile.
 */
struct substrate_uniformly_generated_set substrate_uniformly_gen_set_constructor(
        struct osl_relation_list * uniformly_generated_set)
{
    struct substrate_uniformly_generated_set uni_gen_set = {NULL,NULL,0};
    struct substrate_osl_relation_group_list classes;
    unsigned int i = 0;

    classes = substrate_group_access_relations_by(
            uniformly_generated_set,
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


/**
 * @brief Create an equivalence class profile. Basically it just count the number of
 * references in the class and create/populate/return the correct structure.
 * After a call to this function, the relations list should not be accessed anymore
 * (nor freed).
 *
 * @param[in] array_references The list of access relations belonging to this equivalence class.
 *
 * @return An equivalence class profile.
 */
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



/**
 * @brief Rate the reuse between two reuse profile.
 *
 * How the rating works :
 * - This function try to see if the two reuse profiles have some access relations of the
 *   same array/variable (by comparing the array_id of their array_profiles).
 * - If we find some, that means that we could potentially aggregate the array profiles,
 *   and that some equivalence classes could be aggregated,
 *   so we call \a substrate_rate_array_profiles to rate them.
 * - If not, we count the total number of all the equivalence classes of of the array profiles.
 *
 * In the end, the rating will be equal to : number of references in the aggregated
 * equivalence classes / total number of reference of the reuse profile.
 *
 * @param rp1 The first reuse profile used for the rating.
 * @param rp2 The second reuse profile used for the rating.
 *
 * @return A rating between 0 and 1 (0 meaning that there isn't any reuse, and 1 meaning
 * a perfect reuse).
 */
double substrate_rate_reuse_profiles(
        struct substrate_reuse_profile rp1,
        struct substrate_reuse_profile rp2)
{
    struct reuse_rate rating = {0,0}, tmp = {0,0};
    unsigned int i1 = 0, i2 = 0;

    //We look for match in array id.
    //If we find one, we rate the array profile.
    //If not, we know that 0 references in equivalence classes can be aggregated,
    //and we need to count the number of references in the equivalence classes.
    //
    //After these loops, we know about all the array match between rp1 and rp2, and all the
    //"no-match" of rp1 in rp2. But we don't know about the "no-match" of rp2 in rp1, so
    //we need another loop nest to count references of rp2 for array that don't have a match
    //in rp1.
    for(i1=0; i1<rp1.size ; i1++)
    {
        for(i2=0 ; i2<rp2.size ; i2++)
        {
            if(osl_int_eq(
                        rp1.array_profiles[i1].array_id_precision,
                        rp1.array_profiles[i1].array_id,
                        rp2.array_profiles[i2].array_id))
            {
                tmp = substrate_rate_array_profiles(
                        rp1.array_profiles[i1],
                        rp2.array_profiles[i2]);
                break;
            }
        }
        if(i2 >= rp2.size)
        {
            tmp.nb_same_class_refs = 0;
            tmp.nb_total_refs = substrate_array_profile_count_access(rp1.array_profiles[i1]);
        }

        rating.nb_same_class_refs += tmp.nb_same_class_refs;
        rating.nb_total_refs += tmp.nb_total_refs;
    }
    for(i2=0 ; i2<rp2.size ; i2++)
    {
        for(i1=0; i1<rp1.size ; i1++)
        {
            if(osl_int_eq(
                        rp1.array_profiles[i1].array_id_precision,
                        rp1.array_profiles[i1].array_id,
                        rp2.array_profiles[i2].array_id))
            {
                break;
            }
        }
        if(i1 >= rp1.size)
        {
            tmp.nb_same_class_refs = 0;
            tmp.nb_total_refs = substrate_array_profile_count_access(rp2.array_profiles[i2]);
        }

        rating.nb_total_refs += tmp.nb_total_refs;
    }

    return (double)rating.nb_same_class_refs / (double)rating.nb_total_refs;
}


/**
 * @brief Rate the reuse between two array profile.
 *
 * @param ap1 The first array profile used for the rating.
 * @param ap2 The second array profile used for the rating.
 *
 * @return A reuse_rate corresponding to the number of access relation that would be
 * aggregated (because they belong to the same Uniformly Generated Set and equivalence class)
 * and the total number of access relations of the array profile.
 */
struct reuse_rate substrate_rate_array_profiles(
        struct substrate_array_profile ap1,
        struct substrate_array_profile ap2)
{
    struct reuse_rate rating = {0,0}, tmp = {0,0};
    unsigned int i1 = 0, i2 = 0;

    //After these loops, we know about all the array match between ap1 and ap2, and all the
    //"no-match" of ap1 in ap2. But we don't know about the "no-match" of ap2 in ap1, so
    //we need another loop nest to count references of ap2 for array that don't have a match
    //in ap1.
    for(i1=0; i1<ap1.size ; i1++)
    {
        for(i2=0 ; i2<ap2.size ; i2++)
        {
            if(substrate_H_matrix_eq(
                        ap1.uniformly_gen_sets[i1].H_matrix,
                        ap2.uniformly_gen_sets[i2].H_matrix))
            {
                tmp = substrate_rate_uniformly_generated_sets(
                        ap1.uniformly_gen_sets[i1],
                        ap2.uniformly_gen_sets[i2]);
                break;
            }
        }
        if(i2 >= ap2.size)
        {
            tmp.nb_same_class_refs = 0;
            tmp.nb_total_refs = 
                substrate_uniformly_generated_set_count_access(ap1.uniformly_gen_sets[i1]);
        }

        rating.nb_same_class_refs += tmp.nb_same_class_refs;
        rating.nb_total_refs += tmp.nb_total_refs;
    }
    for(i2=0 ; i2<ap2.size ; i2++)
    {
        for(i1=0; i1<ap1.size ; i1++)
        {
            if(substrate_H_matrix_eq(
                        ap1.uniformly_gen_sets[i1].H_matrix,
                        ap2.uniformly_gen_sets[i2].H_matrix))
            {
                break;
            }
        }
        if(i1 >= ap1.size)
        {
            tmp.nb_same_class_refs = 0;
            tmp.nb_total_refs = 
                substrate_uniformly_generated_set_count_access(ap2.uniformly_gen_sets[i2]);
        }

        rating.nb_total_refs += tmp.nb_total_refs;
    }

    return rating;
}

/**
 * @brief Rate two Uniformly Generated Set profile (with same H matrix)
 * by comparing the number of access relations that have the same equivalence class.
 *
 * @param ugs1 The first profile used for the rating.
 * @param ugs2 The second profile used for the rating.
 *
 * @return A reuse rate composed of : the number of access relations that have the same
 * equivalence class, and the total number of access relations.
 */
struct reuse_rate substrate_rate_uniformly_generated_sets(
        struct substrate_uniformly_generated_set ugs1,
        struct substrate_uniformly_generated_set ugs2)
{
    struct reuse_rate rating = {0,0};
    bool has_hit = true;
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
            rating.nb_same_class_refs += ugs1.classes[i1].size + ugs2.classes[i2].size;
        rating.nb_total_refs += ugs1.classes[i1].size;
    }
    for(i2=0 ; i2<ugs2.size ; i2++)
    {
        has_hit = false;
        for(i1=0; (i1<ugs1.size) && (has_hit == false); i1++)
        {
            has_hit = substrate_access_class_eq(
                            ugs1.classes[i1].array_references->elt,
                            ugs2.classes[i2].array_references->elt);
        }
        if(has_hit == false)
            rating.nb_total_refs += ugs2.classes[i2].size;
    }

    return rating;
}
