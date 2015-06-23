#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "substrate/utils.h"

#include "osl/int.h"
#include "substrate/options.h"

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
 * @brief Clone (hard copy) the n fist osl_scop, also cloning its
 * different field EXCEPT the statements.
 *
 * If n<0 or , all the scop are cloned.
 *
 * @param scop The scops that will be cloned.
 * @param n The number of scops that will be cloned.
 *
 * @return The n hard copy (or less if there is less scops that expected) of scop,
 * without statements.
 */
struct osl_scop * substrate_osl_scop_nclone_except_statement(
        struct osl_scop * scop,
        int n)
{
    struct osl_scop * res = NULL, *new = NULL;
    int i = 0;
    
    if(n < 0)
    {
        n = osl_scop_number(scop);
    }
   
    for(i=0 ; (i<n) && (scop!=NULL) ; i++)
    {
        new = osl_scop_malloc();
        new->version = scop->version;
        OSL_strdup(new->language, scop->language);
        new->context = osl_relation_clone(scop->context);
        new->parameters = osl_generic_clone(scop->parameters);
        new->statement = NULL;
        new->registry = osl_interface_clone(scop->registry);
        new->extension = osl_generic_clone(scop->extension);
        new->usr = scop->usr;
        new->next = NULL;

        scop = scop->next;
        osl_scop_add(&res, new);
    }

    return res;
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

/**
 * @brief Reset the beta depth of a statement (set all beta component
 * to 0).
 *
 * @param[inout] stmt The statement which is reset.
 */
void substrate_reset_beta_depth(struct osl_statement * stmt)
{
    int i = 0;
    struct osl_relation *scattering = NULL;

    scattering = stmt->scattering;
    for(i=0 ; i<scattering->nb_rows; i+=2)
    {
        osl_int_set_si(
                scattering->precision,
                &scattering->m[i][scattering->nb_columns-1],
                0);
    }
}


/**
 * @brief This function returns the first generic structure with a given URI in the generic
 * list provided as parameter and NULL if it doesn't find such a generic.
 *
 * @param[in] x The generic list where to search a given generic URI. 
 * @param[in] URI The URI of the generic we are looking for. 
 *
 * @return The first generic of the requested URI in the list. 
 */
struct osl_generic * substrate_osl_generic_lookup(
        struct osl_generic * x,
        char const * const URI)
{
  while (x != NULL) {
    if (osl_generic_has_URI(x, URI))
      return x;

    x = x->next;
  }

  return NULL;
}


/**
 * @brief This function builds and returns a "hard copy" (not a pointer copy) of the
 * n first elements of an osl_generic_t list.
 *
 * \param generic The pointer to the generic structure we want to clone.
 * \param n       The number of nodes we want to copy (n<0 for infinity).
 * \return The clone of the n first nodes of the generic list.
 *
 * @return 
 */
struct osl_generic * substrate_osl_generic_nclone(
        struct osl_generic * generic,
        int n)
{
    osl_generic_p clone = NULL, new;
    osl_interface_p interface;
    void * x;

    if(n<0)
    {
        clone = osl_generic_clone(generic);
    }
    else
    {
        while ((generic != NULL) && (n>0)) { 
            if (generic->interface != NULL) {
                x = generic->interface->clone(generic->data);
                interface = osl_interface_clone(generic->interface);
                new = osl_generic_malloc();
                new->interface = interface;
                new->data = x;
                osl_generic_add(&clone, new);
            }
            else {
                OSL_warning("unregistered interface, cloning ignored");
            }
            generic = generic->next;
            n--;
        }
    }

    return clone;
}

/**
 * @brief Concatenate two osl_strings into one.
 *
 * @param dest[out] A pointer to the destination osl_strings.
 * @param str1[in] The first osl_strings.
 * @param str2[in] The second osl_strings.
 */
void substrate_osl_strings_concat(
        struct osl_strings ** dest,
        struct osl_strings * str1,
        struct osl_strings * str2)
{
    struct osl_strings * res = NULL;
    unsigned int i = 0;

    res = osl_strings_clone(str1);
    while(str2->string[i] != NULL)
    {
        osl_strings_add(res, str2->string[i]);
        i++;
    }
    
    *dest = res;
}


bool substrate_osl_relation_part_equal(struct osl_relation * rel1,struct osl_relation * rel2)
{
    bool res = true;
    bool is_equal = true;
    int i = 0, j = 0, k = 0;

    if (((rel1 == NULL) && (rel2 != NULL)) ||
            ((rel1 != NULL) && (rel2 == NULL)) ||
            (rel1->type != rel2->type) ||
            (rel1->precision != rel2->precision) ||
            (rel1->nb_rows != rel2->nb_rows) ||
            (rel1->nb_columns != rel2->nb_columns) ||
            (rel1->nb_output_dims != rel2->nb_output_dims) ||
            (rel1->nb_input_dims != rel2->nb_input_dims) ||
            (rel1->nb_local_dims != rel2->nb_local_dims) ||
            (rel1->nb_parameters != rel2->nb_parameters))
        return false;

    bool equal_rows[rel2->nb_rows];
    memset(&equal_rows, false, sizeof(bool) * rel2->nb_rows);

    for (i = 0; (i < rel1->nb_rows) && res ; i++)
    {
        for (j = 0; j < rel2->nb_rows; j++)
        {
            if(equal_rows[j])
                continue;

            is_equal = true;
            for (k = 0; (k < rel2->nb_columns) && is_equal ; k++)
            {
                if (osl_int_ne(rel1->precision, rel1->m[i][k], rel2->m[j][k]))
                    is_equal = false;
            }

            if(is_equal)
            {
                equal_rows[j] = true;
                break;
            }
        }
        if( j >= rel2->nb_rows )
            res = false;
    }

    return res;
}

bool substrate_osl_relation_equal(struct osl_relation * rel1,struct osl_relation * rel2)
{
    bool res = true;
    unsigned int rel_size1 = 0, rel_size2 = 0;
    unsigned int i = 0;
    struct osl_relation * rel_iterator = NULL;


    if (rel1 == rel2)
        return true;


    rel_size1 = osl_relation_count(rel1);
    rel_size2 = osl_relation_count(rel2);

    if (((rel1 == NULL) && (rel2 != NULL)) ||
            ((rel1 != NULL) && (rel2 == NULL)) ||
            (rel_size1 != rel_size2))
        return false;


    struct osl_relation * rel2_array[rel_size2];
    rel_iterator = rel2;
    for (i = 0 ; i<rel_size2 ; i++)
    {
        rel2_array[i] = rel_iterator;
        rel_iterator = rel_iterator->next;
    }

    rel_iterator = rel1;
    while(rel_iterator != NULL)
    {
        for (i = 0 ; i < rel_size2 ; i++)
        {
            if (substrate_osl_relation_part_equal(rel_iterator, rel2_array[i]))
            {
                rel2_array[i] = NULL;
                break;
            }
        }
        if (i >= rel_size2)
        {
            res = false;
            break;
        }
        rel_iterator = rel_iterator->next;
    }
    
    return res;
}

