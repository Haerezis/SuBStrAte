#include <stdlib.h>

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
