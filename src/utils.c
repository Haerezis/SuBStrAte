#include <stdlib.h>
#include <stdbool.h>

#include "utils.h"
#include "osl/int.h"

#define SIZE 16

struct substrate_osl_int_set substrate_array_id_set_from_access_relations(
        struct osl_relation_list * rl)
{
    struct substrate_osl_int_set array_id_set = {NULL,0,0};

    unsigned int i = 0;
    struct osl_relation * current_relation = NULL;
    osl_int_t current_array_id;

    //Initial allocation of the array containing the set
    array_id_set.set = (osl_int_t*) malloc(SIZE * sizeof(osl_int_t));
    array_id_set.max_size = SIZE;

    while(rl != NULL)
    {
        current_relation = rl->elt;
        current_array_id = current_relation->m[0][current_relation->nb_columns-1];

        for(i=0; i<array_id_set.size ; i++)
        {
            if(osl_int_eq(current_relation->precision, array_id_set.set[i],current_array_id))
                break;
        }
        if(i >= array_id_set.size)
        {
            if(array_id_set.size == array_id_set.max_size)
            {
                array_id_set.set = (osl_int_t*) realloc(
                        array_id_set.set,
                        2 * array_id_set.max_size * sizeof(osl_int_t));
                array_id_set.max_size *= 2;
            }
            array_id_set.set[array_id_set.size] = current_array_id;
            array_id_set.size++;
        }

        rl=rl->next;
    }

    return array_id_set;
}

bool substrate_different_H_matrix(
        struct osl_relation * access_relation1,
        struct osl_relation * access_relation2)
{
    bool result = true;
    int l = 0, c = 0;

    //ar is created only to avoid using long named variable like access_relation1
    //its value could have been access_relation1 or access_relation2, because the
    //two relations should share the same nb_rows/nb_output_dims/nb_input_dims
    struct osl_relation * ar = access_relation1;
    
    for(l = 1 ; l < (ar->nb_input_dims+1) ; l++)
    {
        for(c = 1 + ar->nb_output_dims ; c < (1+ar->nb_output_dims+ar->nb_input_dims ; c++)
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

struct osl_relation_list * substrate_remove_duplicate_H_matrix(
        struct osl_relation_list * rl)
{
    struct osl_relation_list
        *returned_list = NULL,
        *current_relation = NULL,
        *tmp = NULL;

    returned_list = osl_relation_list_clone(rl);

    //We don't start with the first item because the first item can't have
    //a duplicate H matrix, because it's the first item of the list.
    current_relation = returned_list;
    while(current_relation->next != NULL)
    {
        //We look from the beginning of the list with tmp if an access matrix contains
        //the same H matrix as the current_relation->next access matrix
        tmp = returned_list;
        while(  (tmp != current_relation->next)
                && substrate_different_H_matrix(tmp->elt, current_relation->next->elt))
        {
            tmp=tmp->next;
        }
        if(tmp != current_relation->next)
        {
            //if that's the case, we remove current_relation->next of the list
            //because it contains the same H matrix as one of its predecessor.
            tmp = current_relation->next;
            current_relation->next = current_relation->next->next;

            tmp->next = NULL;
            osl_relation_list_free(tmp);
            tmp = NULL;
        }
        else
        {
            //if no access relation math the current_relation->next H matrix,
            //then we iterate in the list.
            current_relation = current_relation->next;
        }
    }

    return returned_list;
}
