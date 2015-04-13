#include <stdlib.h>
#include <string.h>

#include "candl/candl.h"

#include "parallelism.h"
#include "utils.h"


struct substrate_parallelism_profile substrate_parallelism_profile_constructor(
       struct osl_scop * scop,
       struct osl_statement * statement)
{
    struct substrate_parallelism_profile res = {NULL,0};
    struct candl_options *candl_options = NULL;
    struct osl_dependence *candl_dep = NULL, *candl_dep_cursor = NULL;
    struct osl_scop *candl_scop = NULL;
    unsigned int i = 0;

    //Preparing and allocating the parallelism structure.
    res.size = statement->domain->nb_output_dims;
    res.loop_carried_dependences = (bool*) malloc(res.size * sizeof(bool));
    memset(res.loop_carried_dependences,false,res.size);


    //Creating a dummy scop with only the current analyzed statement for candl
    candl_scop = substrate_osl_scop_nclone_expect_statement(scop, 1);
    candl_scop->statement = osl_statement_nclone(statement,1);

    //Initiating candl, then using it to generate the dependences,
    //and finally extracting them to use later.
    candl_options = candl_options_malloc();
    candl_scop_usr_init(candl_scop);
    candl_dependence_add_extension(candl_scop, candl_options);
    candl_dep = osl_generic_lookup(candl_scop->extension, OSL_URI_DEPENDENCE);

    //For every loop, from outer to inner, we check if there is a loop carried dependences.
    //If that's the case, the statement can't be parallelized for this loop.
    for(i=0 ; i<res.size ; i++)
    {
        //If the candl_dep is NULL, it means that there is no dependences whatsoever in the statement,
        //we don't have to check with candl_dependence_is_loop_carried.
        //res.loop_carried_dependences is already set to false for every loop index.
        candl_dep_cursor = candl_dep;
        while((candl_dep_cursor != NULL) && (res.loop_carried_dependences[i] == 0))
        {
            res.loop_carried_dependences[i] = 
                candl_dependence_is_loop_carried( candl_dep_cursor, i);

            candl_dep_cursor = candl_dep_cursor->next;
        }
    }

    candl_options_free(candl_options);
    candl_scop_usr_cleanup(candl_scop);
    osl_scop_free(candl_scop);

    return res;
}

double substrate_rate_parallelism_profiles(
        struct substrate_parallelism_profile pp1,
        struct substrate_parallelism_profile pp2)
{
    double res = 0.0;
    unsigned int nb_parallel_loop_in_common = 0, nb_parallel_loops_total = 0;
    unsigned int tmp1 = 0, tmp2 = 0;
    unsigned int i = 0;

    //XXX should I force 2 statement to have the same loop depth ?
    if(pp1.size == pp2.size)
    {
        nb_parallel_loops_total = pp1.size;
        for(i=0 ; i<pp1.size ; i++)
        {
            tmp1 = pp1.loop_carried_dependences[i] == true ? 1 : 0;
            tmp2 = pp2.loop_carried_dependences[i] == true ? 1 : 0;
            nb_parallel_loops_total = tmp1 + tmp2;
            if((tmp1 == tmp2) && (tmp1 == 1))
            {
                nb_parallel_loop_in_common+=2;
            }
        }
    }
    res = ((double)nb_parallel_loop_in_common) / ((double)nb_parallel_loops_total);
    return res;
}
