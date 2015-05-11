#include <stdlib.h>

#include "substrate/optimization.h"

#include "substrate/options.h"
#include "substrate/reuse.h"
#include "substrate/utils.h"
#include "osl/osl.h"

/**
 * @brief Compute the weighted average of all the different profile rate.
 *
 * @param reuse_rate The reuse rate.
 * @param parallelism_rate The parallelism rate.
 *
 * @return The weighted average.
 */
double substrate_weighted_rate(
        double reuse_rate,
        double parallelism_rate)
{
    double weight_sum = 0.0;
    double weighted_rate_sum = 0.0;

    weighted_rate_sum = g_substrate_options.reuse_weight * reuse_rate +
        g_substrate_options.parallelism_weight * parallelism_rate;

    weight_sum = g_substrate_options.reuse_weight +
        g_substrate_options.parallelism_weight;

    return weighted_rate_sum / weight_sum; 
}

/**
 * @brief Optimize a scop by aggregating similar behaving statements, using the
 * statements profiles contained in \a profiled_scop.
 *
 * @param[in] profiled_scop A scop that has been analyzed and profiled (the result of the
 * \a analyze function).
 *
 * @return an osl_scop corresponding to the optimization of \a profiled_scop.
 */
struct osl_scop * substrate_optimize(
        struct osl_scop * profiled_scop)
{
    struct osl_scop * res = NULL, *original_res = NULL;

    res = substrate_osl_scop_clone(profiled_scop);
    original_res = res;
    while(res != NULL)
    {
        substrate_successive_statements_optimization(res);
        res = res->next;
    }

    //return the pointer to the first osl_scop of the list,
    //not res directly (that point to the last)
    return original_res;
}




/**
 * @brief Optimize a scop by aggregating (if possible and preferable)
 * successive statements one by one.
 *
 * @param[inout] profiled_scop A scop profile that will be optimized.
 */
void substrate_successive_statements_optimization(struct osl_scop * profiled_scop)
{
    struct osl_statement 
        *stmt = NULL,// Will point to the statement BEFORE the first statement that is rated/aggregated.
        *stmt_original = NULL,// Point to the original address of stmt after its empty alloc.
        *stmt1 = NULL,// Point to the first statement that is rated/aggregated
        *stmt2 = NULL,// Point to the second statement that is rated/aggregated
        *stmt_fusion = NULL;// Point to the new statement created when stmt1 and stmt2 are aggregated.
    struct substrate_statement_profile *stmt_profile1 = NULL, *stmt_profile2;
    bool same_domain = false, same_scattering = false;
    double reuse_rate = 0.0, parallelism_rate = 0.0;
    double weighted_rate = 0.0;

    //if there isn't even 2 statements, we can't aggregate anything, so we return
    if((profiled_scop->statement == NULL) || (profiled_scop->statement->next == NULL))
        return;

    //We just create an empty osl_statement to not break the iteration pattern.
    stmt = osl_statement_malloc();
    stmt_original = stmt;//to keep track of the empty osl_statement, to free it at the end.
    stmt->next = profiled_scop->statement;
    while(stmt->next->next != NULL)
    {
        //stmt1 point to the first statement that will be rated and maybe aggregated.
        //stmt2 point to the second statement that will be rated and maybe aggregated.
        stmt1 = stmt->next;
        stmt2 = stmt->next->next;

        same_domain = osl_relation_equal(
                stmt1->domain,
                stmt2->domain);

        if(same_domain == true)
        {
            same_scattering = substrate_same_scattering_and_beta_depth(
                    stmt1->scattering,
                    stmt2->scattering);

            if(same_scattering == true)
            {
                stmt_profile1 = (struct substrate_statement_profile*)stmt1->usr;
                stmt_profile2 = (struct substrate_statement_profile*)stmt2->usr;


                reuse_rate = substrate_rate_reuse_profiles(
                        stmt_profile1->reuse,
                        stmt_profile2->reuse);
                parallelism_rate = substrate_rate_parallelism_profiles(
                        stmt_profile1->parallelism,
                        stmt_profile2->parallelism);

                weighted_rate = substrate_weighted_rate(
                        reuse_rate,
                        parallelism_rate);
                if(weighted_rate >= g_substrate_options.minimal_rate)
                {
                    //aggregate stmt1 and stmt2 into stmt_fusion.
                    stmt_fusion = substrate_statement_fusion(profiled_scop, stmt1, stmt2);
                    //replace stmt1 and stmt2 in the list by stmt_fusion
                    stmt->next = stmt_fusion;
                    stmt2->next = NULL;
                    substrate_statement_profile_free(stmt_profile1);
                    substrate_statement_profile_free(stmt_profile2);
                    osl_statement_free(stmt1);//this function normaly also free stmt2
                }
                else
                {
                    stmt = stmt->next;
                }
            }
            else
            {
                //TODO Use candl and simulate a move of the 2nd statement at the same
                //scattering and beta depth than the 1st (using the base and modified scattering)
                stmt = stmt->next;
            }
        }
        else
        {
            stmt = stmt->next;
        }
    }
    //If the 1st statement of the list has been aggregated, a new osl_statement has been
    //created and used to replace the 1st and 2nd statement, so we need to properly
    //point profiled_scop->statement to the address of this new osl_statement, which is in
    //stmt_original->next.
    profiled_scop->statement = stmt_original->next;

    //After that we need to free the empty osl_statement, because it's useless now.
    stmt_original->next = NULL;
    osl_statement_free(stmt_original);
}
