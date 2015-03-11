#include <stdlib.h>
#include <stdbool.h>

#include "utils.h"
#include "osl/int.h"

#define SIZE 16


void substrate_osl_relation_list_set_realloc(
        struct substrate_osl_relation_list_set * set)
{
    if(set->size == set->max_size)
    {
        set->set = (struct osl_relation_list**) realloc(
                set->set,
                2 * set->max_size * sizeof(struct osl_relation_list*));
        set->max_size *= 2;
    }
}


osl_int_t substrate_get_array_id_from_access_relation(
        struct osl_relation * rel)
{
    return rel->m[0][rel->nb_columns-1];
}


struct substrate_osl_relation_list_set substrate_group_access_relations_by_array(
        struct osl_relation_list * rl)
{
    struct substrate_osl_relation_list_set access_relations_groups = {NULL,0,0};

    unsigned int i = 0;
    struct osl_relation * current_relation = NULL;
    struct osl_relation_list * next_rel_list = NULL;
    struct osl_relation_list * tmp_rel_list = NULL;
    osl_int_t current_array_id;
    osl_int_t tmp_array_id;

    //Initial allocation of the array containing the set
    access_relations_groups.max_size = SIZE;
    access_relations_groups.size = 0;
    access_relations_groups.set = (struct osl_relation_list**)
        malloc(SIZE * sizeof(struct osl_relation_list*));


    while(rl != NULL)
    {
        //unlink rl from the main list because it will be added to a group later,
        //as the last elt of the group/list, so it need have its next field at NULL
        next_rel_list = rl->next;
        rl->next = NULL;

        current_relation = rl->elt;
        current_array_id = substrate_get_array_id_from_access_relation(current_relation);

        //We check if there's already a group for the array of the current access relation
        for(i=0; i<access_relations_groups.size ; i++)
        {
            tmp_array_id = substrate_get_array_id_from_access_relation(
                    access_relations_groups.set[i]->elt);

            if(osl_int_eq(current_relation->precision, tmp_array_id, current_array_id))
                break;
        }
        if(i >= access_relations_groups.size)
        {
            //If no group has been found, we need to create a new one (and possibly
            //increase the size of the set of groups).
            
            substrate_osl_relation_list_set_realloc(&access_relations_groups);
            
            access_relations_groups.set[access_relations_groups.size] = rl;
            access_relations_groups.size++;
        }
        else
        {
            //If a group was found, we iterate to the end of the list
            //to add the new elt to the tail
            tmp_rel_list = access_relations_groups.set[i];
            while(tmp_rel_list->next != NULL) tmp_rel_list = tmp_rel_list->next;
            tmp_rel_list->next = rl;
        }
        
        rl = next_rel_list;
    }

    return access_relations_groups;
}



struct substrate_osl_relation_list_set substrate_group_access_relations_by_H_matrix(
        struct osl_relation_list * rl)
{
    struct substrate_osl_relation_list_set uniformly_generated_set_list = {NULL,0,0};
    struct osl_relation_list
        *next_relation_list = NULL,
        *tmp = NULL;
    unsigned int i = 0;

    //Initial allocation of the array containing the set
    uniformly_generated_set_list.max_size = SIZE;
    uniformly_generated_set_list.size = 0;
    uniformly_generated_set_list.set = (struct osl_relation_list**)
        malloc(SIZE * sizeof(struct osl_relation_list*));

    while(rl != NULL)
    {
        //unlink rl from the main list because it will be added to a group later,
        //as the last elt of the group/list, so it need have its next field at NULL
        next_relation_list = rl->next;
        rl->next = NULL;

        //We look all the already created Uniformly Generated Set (access matrix with a same H, Wolf91)
        //if there is one with the same H matrix of the current access matrix.
        for(i=0; i<uniformly_generated_set_list.size ; i++)
        {
            if(substrate_H_matrix_eq(uniformly_generated_set_list.set[i]->elt, rl->elt))
                break;
        }

        if(i < uniformly_generated_set_list.size)
        {
            //If we've found an already created Uniformly Generated Set for this H matrix,
            //we just add the current access matrix to the tail of the list (of the set).

            //We iterate to the end of the list to add the new elt to the tail
            tmp = uniformly_generated_set_list.set[i];
            while(tmp->next != NULL) tmp = tmp->next;

        }
        else
        {
            //If we haven't found an already created Unfirmly Generated Set,
            //we just create one by adding the current access matrix to the next
            //spot in the list.
            
            //We just check that there is still space for a new group/set, if not
            //a realloc is performed.
            substrate_osl_relation_list_set_realloc(&uniformly_generated_set_list);
            
            uniformly_generated_set_list.set[uniformly_generated_set_list.size] = rl;
            uniformly_generated_set_list.size++;

        }

        rl = next_relation_list;
    }

    return uniformly_generated_set_list;
}









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
   
    //The H matrix start at line 1 to line ar->nb_intput_dims included, and start
    //at column 1+nb_output_dims to column 1+nb_output_dims+nb_input_dims.
    //The size of the matrix should be nb_input_dims*nb_input_dims
    for(l = 1 ; l < (ar->nb_input_dims+1) ; l++)
    {
        for(c = 1 + ar->nb_output_dims ; c < (1+ar->nb_output_dims+ar->nb_input_dims) ; c++)
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

bool substrate_H_matrix_neq(
        struct osl_relation * access_relation1,
        struct osl_relation * access_relation2)
{
    return ! substrate_H_matrix_eq(access_relation1,access_relation2);
}
