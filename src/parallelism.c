#include <stdlib.h>
#include <string.h>

#include "candl/candl.h"

#include "substrate/parallelism.h"
#include "substrate/utils.h"


/**
 * @brief Create the parallelism profile of a statement by analyzing its
 * self-dependences (computed by candl).
 *
 * @param[in] scop      The scop of the statement (needed for candl).
 * @param[in] statement The statement that is analyzed.
 *
 * @return A substrate_parallelism_profile containing the parallelism
 * profile of the statement.
 */
struct substrate_parallelism_profile substrate_parallelism_profile_constructor(
       struct osl_scop * scop,
       struct osl_statement * statement)
{
    struct substrate_parallelism_profile res = {NULL,0};
    struct candl_options *candl_options = NULL;
    struct osl_dependence *candl_dep = NULL, *candl_dep_cursor = NULL;
    struct osl_scop *candl_scop = NULL;
    unsigned int i = 0;
    int is_loop_carried = 0;

    //Preparing and allocating the parallelism structure.
    res.size = statement->domain->nb_output_dims;
    res.parallelizable_loops = (bool*) malloc(res.size * sizeof(bool));
    memset(res.parallelizable_loops,false,res.size);


    //Creating a dummy scop with only the current analyzed statement for candl.
    //Also reset the beta depth of the statement, because it's supposed to be
    //alone now.
    candl_scop = substrate_osl_scop_nclone_except_statement(scop, 1);
    candl_scop->statement = osl_statement_nclone(statement,1);
    substrate_reset_beta_depth(candl_scop->statement);

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
        //And res.parallelizable_loops[i] is then set to true for every loop index.
        candl_dep_cursor = candl_dep;
        is_loop_carried = 0;
        while((candl_dep_cursor != NULL) && (is_loop_carried == 0))
        {
            is_loop_carried = is_loop_carried || 
                candl_dependence_is_loop_carried( candl_dep_cursor, i);

            candl_dep_cursor = candl_dep_cursor->next;
        }
        res.parallelizable_loops[i] = !is_loop_carried;
    }

    candl_options_free(candl_options);
    candl_scop_usr_cleanup(candl_scop);
    osl_scop_free(candl_scop);

    return res;
}

/**
 * @brief Rate the similarity between two statements' parallelism profile.
 * The rate is a double comprised between 0 and 1 (0 being totally NOT similar,
 * and 1 being TOTALLY similar).
 *
 * @param pp1 The first parallelism profile.
 * @param pp2 The second parallelism profile.
 *
 * @return A double comprised between 0 and 1 rating the similarity between
 * the two profiles.
 */
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
            tmp1 = pp1.parallelizable_loops[i] == true ? 1 : 0;
            tmp2 = pp2.parallelizable_loops[i] == true ? 1 : 0;
            nb_parallel_loops_total = tmp1 + tmp2;
            if((tmp1 == tmp2) && (tmp1 == 1))
            {
                nb_parallel_loop_in_common+=2;
            }
        }
    }
    //avoid the divided by 0 error : if there is no parallel loop, nb_parallel_loop_in_common
    //must be equal to 0, and 0/1 → 0.
    if(nb_parallel_loops_total == 0) nb_parallel_loops_total = 1;
    res = ((double)nb_parallel_loop_in_common) / ((double)nb_parallel_loops_total);

    return res;
}
