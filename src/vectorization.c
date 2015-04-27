#include <stdlib.h>
#include <string.h>

#include "candl/candl.h"

#include "options.h"
#include "vectorization.h"
#include "utils.h"


struct substrate_vectorization_profile substrate_vectorization_profile_constructor(
       struct osl_scop * scop,
       struct osl_statement * statement)
{
    struct substrate_vectorization_profile res = {NULL,0};
    
    unsigned int i = 0;
    
    struct osl_relation_list *relation_list_cursor = NULL;
    struct osl_relation *relation_cursor = NULL;
    unsigned col = 0;
    unsigned line = 0;
    osl_int_t val;
    unsigned common_iterators_vectorized_dim[statement->domain->nb_output_dims];
    int precision = statement->domain->precision;

    struct candl_options *candl_options = NULL;
    struct osl_dependence *candl_dep = NULL, *candl_dep_cursor = NULL;
    struct osl_scop *candl_scop = NULL;


    //Preparing and allocating the vectorization structure.
    res.size = statement->domain->nb_output_dims;
    res.vectorizable_loops = (bool*) malloc(res.size * sizeof(bool));
    memset(res.vectorizable_loops,false,res.size);

    //For every possible iterator, we select only those that are used in the
    //vectorized output dimension (the first if column major, last if row major).
    //
    //If an access relation of the statement doesn't used the i^th iterator,
    //then this iterator can be used to vectorize the statement.
    for(i=0 ; i<res.size ; i++)
    {
        common_iterators_vectorized_dim[i] = true;
        relation_list_cursor = statement->access;
        while((relation_list_cursor != NULL) && (common_iterators_vectorized_dim[i]))
        {
            relation_cursor = relation_list_cursor->elt;
            while((relation_cursor != NULL) && (common_iterators_vectorized_dim[i]))
            {
                col = g_substrate_options.row_major ? relation_cursor->nb_output_dims-1 : 1;
                line = candl_util_relation_get_line(relation_cursor, col);

                val = relation_cursor->m[line][1 + relation_cursor->nb_output_dims + i];
                common_iterators_vectorized_dim[i] = osl_int_zero(precision, val);
                
                relation_cursor = relation_cursor->next;
            }
            relation_list_cursor = relation_list_cursor->next;
        }
    }


    //Creating a dummy scop with only the current analyzed statement for candl
    candl_scop = substrate_osl_scop_nclone_expect_statement(scop, 1);
    candl_scop->statement = osl_statement_nclone(statement,1);

    //Initiating candl, then using it to generate the dependences,
    //and finally extracting them to use later.
    candl_options = candl_options_malloc();
    candl_scop_usr_init(candl_scop);
    candl_dependence_add_extension(candl_scop, candl_options);
    candl_dep = osl_generic_lookup(candl_scop->extension, OSL_URI_DEPENDENCE);

    for(i=0 ; i<res.size; i++)
    {
        if( common_iterators_vectorized_dim[i] == false ) continue;

        res.vectorizable_loops[i] = true;
        candl_dep_cursor = candl_dep;
        while( (candl_dep_cursor != NULL) && (res.vectorizable_loops[i]) )
        {
            if(candl_dependence_is_loop_carried(candl_dep_cursor, i))
            {
                switch(candl_dep_cursor->type)
                {
                    //If the dependence is RaR (which should not be encountered because
                    //we only analyze the self-dependence of the statement) or WaR,
                    //then we don't care : these types doesn't prevent vectorization.
                    case OSL_DEPENDENCE_RAR :
                    case OSL_DEPENDENCE_WAR :
                        break;
                    
                    //If a dependence is RaW or WaW, the i^th cannot be vectorized
                    //because these type of dependence prevent it.
                    case OSL_DEPENDENCE_RAW :
                    case OSL_DEPENDENCE_WAW :
                        res.vectorizable_loops[i] = false;
                        break;
                    default :
                        OSL_error("In function substrate_vectorization_profile_constructor :"
                                " an osl_dependence without a type has been encountered");
                        break;
                }
            }
            candl_dep_cursor = candl_dep_cursor->next;
        }
    }

    candl_options_free(candl_options);
    candl_scop_usr_cleanup(candl_scop);
    osl_scop_free(candl_scop);

    return res;
}

double substrate_rate_vectorization_profiles(
        struct substrate_vectorization_profile vp1,
        struct substrate_vectorization_profile vp2)
{
    double res = 0.0;
    unsigned int nb_vectorizable_loop_in_common = 0, nb_vectorizable_loops_total = 0;
    unsigned int tmp1 = 0, tmp2 = 0;
    unsigned int i = 0;

    //XXX should I force 2 statement to have the same loop depth ?
    if(vp1.size == vp2.size)
    {
        nb_vectorizable_loops_total = vp1.size;
        for(i=0 ; i<vp1.size ; i++)
        {
            tmp1 = vp1.vectorizable_loops[i] == true ? 1 : 0;
            tmp2 = vp2.vectorizable_loops[i] == true ? 1 : 0;
            nb_vectorizable_loops_total = tmp1 + tmp2;
            if((tmp1 == tmp2) && (tmp1 == 1))
            {
                nb_vectorizable_loop_in_common+=2;
            }
        }
    }
    res = ((double)nb_vectorizable_loop_in_common) / ((double)nb_vectorizable_loops_total);
    return res;
}
