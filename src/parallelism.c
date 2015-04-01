#include <stdlib.h>
#include <string.h>

#include "candl/candl.h"

#include "parallelism.h"
#include "utils.h"


struct substrate_parallelism_profile substrate_parallelism_profile_constructor(
       struct osl_statement * statement,
       struct osl_scop * scop)
{
    struct substrate_parallelism_profile res = {NULL,0};
    struct candl_options *candl_options = NULL;
    struct osl_dependence *candl_dep = NULL;
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
        res.loop_carried_dependences[i] = 
            candl_dependence_is_loop_carried(candl_scop,candl_dep,i);
    }

    return res;
}

double substrate_rate_parallelism_profiles(
        struct substrate_parallelism_profile pp1,
        struct substrate_parallelism_profile pp2)
{
    double res = 1.0;
    return res;
}
