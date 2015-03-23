#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "utils.h"

#include "osl/int.h"
#include "options.h"

#define SIZE 16


/**
 * @brief If the array in substrate_osl_relation_group_list is full, this function will
 * reallocate it, increasing its size times 2.
 *
 * @param[inout] group_list The substrate_osl_relation_group_list that will be checked and maybe
 * realloc.
 */
void substrate_osl_relation_group_list_realloc(
        struct substrate_osl_relation_group_list * group_list)
{
    if(group_list->size == group_list->max_size)
    {
        group_list->list = (struct osl_relation_list**) realloc(
                group_list->list,
                2 * group_list->max_size * sizeof(struct osl_relation_list*));
        group_list->max_size *= 2;
    }
}


/**
 * @brief Get the array id from an access relation.
 *
 * @param[in] rel The access relation from which we will get the array id.
 *
 * @return The array id of the access relation \a rel.
 */
osl_int_t substrate_get_array_id_from_access_relation(
        struct osl_relation * rel)
{
    return rel->m[0][rel->nb_columns-1];
}


/**
 * @brief Classify the relations in a relation list by using \a by_function that
 * when given two relations tell us if they need to be the same group/class.
 * \a rl should not be accessed after a call to this function.
 *
 * @param[inout] rl The list of relation that will be classify.
 * @param[in] by_function The function used to group the relations in \a rl. Should return true
 * if two relations have to be grouped together, false otherwise.
 *
 * @return A substrate_osl_relation_group_list containing an array of relation list,
 * each list of the array representing a group/class.
 */
struct substrate_osl_relation_group_list substrate_group_access_relations_by(
        struct osl_relation_list * rl,
        bool (*by_function) (struct osl_relation *,struct osl_relation *))
{
    struct substrate_osl_relation_group_list access_relations_groups = {NULL,0,0};

    unsigned int i = 0;
    struct osl_relation_list * next_rel_list = NULL;
    struct osl_relation_list * tmp_rel_list = NULL;

    //Initial allocation of the array containing the group_list/list of group
    access_relations_groups.max_size = SIZE;
    access_relations_groups.size = 0;
    access_relations_groups.list = (struct osl_relation_list**)
        malloc(SIZE * sizeof(struct osl_relation_list*));


    while(rl != NULL)
    {
        //unlink rl from the main list because it will be added to a group later,
        //as the last elt of the group/list (or first if new group),
        //so it need have its next field at NULL
        next_rel_list = rl->next;
        rl->next = NULL;

        //We check if there's already a group that match the current access relation
        //To match to a group, the current access relation and the first access relation
        //of the group need to return 1 when by_function is applied to them.
        for(i=0; i<access_relations_groups.size ; i++)
        {
            if((*by_function)(rl->elt, access_relations_groups.list[i]->elt))
                break;
        }
        if(i >= access_relations_groups.size)
        {
            //If no group has been found, we need to create a new one (and possibly
            //increase the size of the group_list of groups).
            substrate_osl_relation_group_list_realloc(&access_relations_groups);
            
            access_relations_groups.list[access_relations_groups.size] = rl;
            access_relations_groups.size++;
        }
        else
        {
            //If a group was found, we iterate to the end of the list
            //to add the new elt to the tail
            tmp_rel_list = access_relations_groups.list[i];
            while(tmp_rel_list->next != NULL) tmp_rel_list = tmp_rel_list->next;
            tmp_rel_list->next = rl;
        }
        
        rl = next_rel_list;
    }

    return access_relations_groups;
}



/**
 * @brief Check if two access relations have the same array id.
 *
 * @param[in] access_relation1
 * @param[in] access_relation2
 *
 * @return True if \a access_relation1 and \a access_relation2 have the same array id,
 * false otherwise.
 */
bool substrate_array_id_eq(
        struct osl_relation * access_relation1,
        struct osl_relation * access_relation2)
{
    osl_int_t array1_id;
    osl_int_t array2_id;
    
    array1_id= substrate_get_array_id_from_access_relation(access_relation1);
    array2_id= substrate_get_array_id_from_access_relation(access_relation2);

    return osl_int_eq(access_relation1->precision, array1_id, array2_id);
}

/**
 * @brief Check if two access relation have the same H matrix (ie belong to the same
 * Uniformly Generated Set, Wolf91) or not.
 *
 * @param[in] access_relation1
 * @param[in] access_relation2
 *
 * @return True if the two access relation have the same H matrix, false otherwise.
 */
bool substrate_H_matrix_eq(
        struct osl_relation * access_relation1,
        struct osl_relation * access_relation2)
{
    bool result = true;
    int l = 0, c = 0;

    //ar is created only to avoid using long named variable like access_relation1
    //its value could have been access_relation1 or access_relation2, because the
    //two relations should share the same nb_rows/nb_output_dims/nb_input_dims
    struct osl_relation * ar = access_relation1;
   
    //The H matrix start at line 1 to line ar->nb_input_dims included, and start
    //at column 1+nb_output_dims to column 1+nb_output_dims+nb_input_dims.
    //The size of the matrix should be nb_input_dims*nb_input_dims
    for(l = 1 ; l < ar->nb_output_dims ; l++)
    {
        for(c = 1 + ar->nb_output_dims ; c<(1+ar->nb_output_dims+ar->nb_input_dims) ; c++)
        {
            if(osl_int_ne(
                        ar->precision,
                        access_relation1->m[l][c],
                        access_relation2->m[l][c]))
            {
                result = false;
                break;
            }
        }
    }

    return result;
}

/**
 * @brief Check if two access relation have different H matrix (ie belong to different
 * Uniformly Generated Set).
 *
 * @param[in] access_relation1
 * @param[in] access_relation2
 *
 * @return 
 */
bool substrate_H_matrix_neq(
        struct osl_relation * access_relation1,
        struct osl_relation * access_relation2)
{
    return ! substrate_H_matrix_eq(access_relation1,access_relation2);
}


/**
 * @brief Check if two access relation belonging to the same Uniformly Generated Set also
 * belong to the same equivalence class.
 *
 * The two access relation NEED to belong to the same Uniformly Generated Set, because
 * only the constant and parameters part of the relation is checked, not the H matrix.
 *
 * @param[in] access_relation1
 * @param[in] access_relation2
 *
 * @return True if the two relations belong to the same equivalence class, false otherwise.
 */
bool substrate_access_class_eq(
        struct osl_relation * access_relation1,
        struct osl_relation * access_relation2)
{
    bool result = true;
    int l = 0, c = 0;
    int row_major = 0, column_major = 0;

    //ar is created only to avoid using long named variable like access_relation1
    //its value could have been access_relation1 or access_relation2, because the
    //two relations should share the same nb_rows/nb_output_dims/nb_input_dims
    struct osl_relation * ar = access_relation1;

    row_major = g_substrate_options.row_major ? 1 : 0;
    column_major = g_substrate_options.row_major ? 0 : 1;
   
    for(l = 1+column_major ; l < (ar->nb_output_dims-row_major) ; l++)
    {
        for(c=(ar->nb_columns - ar->nb_parameters - 1) ; c < ar->nb_columns ; c++)
        {
            if(osl_int_ne(ar->precision,access_relation1->m[l][c],access_relation2->m[l][c]))
            {
                result = false;
                break;
            }
        }
    }

    return result;
}


/**
 * @brief dest := source 
 *
 * Copy \a source in \dest, cloning/copying all source fields (except source->statement).
 *
 * @param[out] dest
 * @param[in] source
 */
void substrate_copy_scop_except_statements(
        struct osl_scop * dest,
        struct osl_scop * source)
{
    dest->version = source->version;
    dest->language = malloc(strlen(source->language)+1);//alloc the string
    strcpy(dest->language,source->language);//copy the string after alloc
    dest->context = osl_relation_clone(source->context);
    dest->parameters = osl_generic_clone(source->parameters);
    dest->statement = NULL;
    dest->registry = osl_interface_clone(source->registry);
    dest->extension = osl_generic_clone(source->extension);
    dest->usr = NULL;
    dest->next = NULL;//TODO : substrate doesn't handle files with multiples scope for now
}


/**
 * @brief Check if two scattering relation are the same, and if the beta depth is
 * the same (the last beta component can be different).
 *
 * @param[in] rel1
 * @param[in] rel2
 *
 * @return True if the two relations are the same scattering relation and the beta depth
 * the same, false otherwise.
 */
bool substrate_same_scattering_and_beta_depth(
        struct osl_relation * rel1,
        struct osl_relation * rel2)
{
    int l = 0, c = 0;
    bool res = true;

    if((rel1->nb_output_dims == rel2->nb_output_dims)
            && (rel1->nb_input_dims == rel2->nb_input_dims)
            && (rel1->nb_parameters == rel2->nb_parameters)
            && (rel1->nb_local_dims == rel2->nb_local_dims))
    {
        //avoiding the last row, because it represents the last beta component of the
        //scattering, used to order the statements placed at the same depth
        for(l = 0 ; (l<(rel1->nb_output_dims-1)) && (res==true) ; l++)
        {
            for(c = 1 ; (c<rel1->nb_columns) && (res==true) ; c++)
            {
                res = osl_int_eq(rel1->precision, rel1->m[l][c], rel2->m[l][c]);
            }
        }
    }
    else
    {
        res = false;
    }

    return res;
}
