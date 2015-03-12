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


struct substrate_osl_relation_list_set substrate_group_access_relations_by(
        struct osl_relation_list * rl,
        bool (*by_function) (struct osl_relation *,struct osl_relation *))
{
    struct substrate_osl_relation_list_set access_relations_groups = {NULL,0,0};

    unsigned int i = 0;
    struct osl_relation * current_relation = NULL;
    struct osl_relation_list * next_rel_list = NULL;
    struct osl_relation_list * tmp_rel_list = NULL;

    //Initial allocation of the array containing the set/list of group
    access_relations_groups.max_size = SIZE;
    access_relations_groups.size = 0;
    access_relations_groups.set = (struct osl_relation_list**)
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
            if((*by_function)(rl->elt, access_relations_groups.set[i]->elt))
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


bool substrate_temporal_class_eq(
        struct osl_relation * access_relation1,
        struct osl_relation * access_relation2)
{
    return true;//TODO
}

bool substrate_spatial_class_eq(
        struct osl_relation * access_relation1,
        struct osl_relation * access_relation2)
{
    return true;//TODO
}
