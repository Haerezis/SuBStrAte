#include <stdlib.h>
#include <string.h>

#include "substrate/optimization.h"

#include "substrate/options.h"
#include "substrate/reuse.h"
#include "substrate/utils.h"
#include "substrate/adjacency_matrix.h"
#include "osl/osl.h"
#include "candl/candl.h"
#include "clay/beta.h"
#include "clay/util.h"

/**
 * @brief Compute the weighted average of all the different profile rate.
 *
 * @param reuse_rate The reuse rate.
 * @param parallelism_rate The parallelism rate.
 * @param vectorization_rate The vectorization rate.
 * @param tiling_hyperplane_rate The tiling hyperplane rate.
 *
 * @return The weighted average.
 */
double substrate_weighted_rate(
        double reuse_rate,
        double parallelism_rate,
        double vectorization_rate,
        double tiling_hyperplane_rate)
{
    double weight_sum = 0.0;
    double weighted_rate_sum = 0.0;

    weighted_rate_sum = 
        g_substrate_options.reuse_weight * reuse_rate +
        g_substrate_options.parallelism_weight * parallelism_rate +
        g_substrate_options.vectorization_weight * vectorization_rate +
        g_substrate_options.tiling_hyperplane_weight * tiling_hyperplane_rate;

    weight_sum = 
        g_substrate_options.reuse_weight +
        g_substrate_options.parallelism_weight +
        g_substrate_options.vectorization_weight + 
        g_substrate_options.tiling_hyperplane_weight;

    return weighted_rate_sum / weight_sum; 
}

/**
 * @brief Rate the similarity between 2 statements
 *
 * @param stmt1 first statement.
 * @param stmt2 second statement.
 *
 * @return The similarity rate between stmt1 and stmt2.
 */
double substrate_similarity_rate(
        struct substrate_statement_profile * stmt_profile1,
        struct substrate_statement_profile * stmt_profile2)
{
    double res = 0.0;
    double reuse_rate = 0.0,
           parallelism_rate = 0.0,
           vectorization_rate = 0.0,
           tiling_hyperplane_rate = 0.0;

    reuse_rate = substrate_rate_reuse_profiles(
            stmt_profile1->reuse,
            stmt_profile2->reuse);
    parallelism_rate = substrate_rate_parallelism_profiles(
            stmt_profile1->parallelism,
            stmt_profile2->parallelism);
    vectorization_rate = substrate_rate_vectorization_profiles(
            stmt_profile1->vectorization,
            stmt_profile2->vectorization);
    tiling_hyperplane_rate = substrate_rate_tiling_hyperplane_profiles(
            stmt_profile1->tiling_hyperplane,
            stmt_profile2->tiling_hyperplane);

    res = substrate_weighted_rate(
            reuse_rate,
            parallelism_rate,
            vectorization_rate,
            tiling_hyperplane_rate);

    return res;
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
        *stmt2 = NULL;// Point to the second statement that is rated/aggregated
    bool same_domain = false, same_scattering = false;
    struct substrate_statement_profile *stmt_profile1 = NULL, *stmt_profile2 = NULL;
    double similarity_rate = 0.0;

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
                stmt_profile1 = (struct substrate_statement_profile*) stmt1->usr;
                stmt_profile2 = (struct substrate_statement_profile*) stmt2->usr;
                similarity_rate = substrate_similarity_rate(stmt_profile1, stmt_profile2);
                if(similarity_rate >= g_substrate_options.minimal_rate)
                {
                    substrate_statement_fusion_and_replace(
                            profiled_scop,
                            stmt,
                            stmt1,
                            stmt1,
                            stmt2,
                            stmt);
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


void substrate_move_statement_scattering(
        struct osl_scop * scop,
        struct osl_statement *src_stmt,
        struct osl_statement *dst_stmt,
        bool before)
{
    struct clay_array *dst_beta = NULL;

    if ((src_stmt == NULL) || (dst_stmt == NULL))
    {
        OSL_error("substrate_move_statement : NULL pointer to osl_statement");
    }

    if ((src_stmt->scattering->next != NULL) || (dst_stmt->scattering->next != NULL))
        OSL_error("substrate_move_statement : union of scattering relation not supported yet");

    dst_beta = clay_beta_extract(dst_stmt->scattering);

    clay_util_scattering_update_beta(src_stmt->scattering, dst_beta);
    if (before)
    {
        clay_beta_shift_before(scop->statement, dst_beta, dst_beta->size);
    }
    else
    {
        clay_beta_shift_after(scop->statement, dst_beta, dst_beta->size);
       
        osl_int_increment(src_stmt->scattering->precision, 
              &src_stmt->scattering->m[(dst_beta->size-1) * 2][src_stmt->scattering->nb_columns-1],
              src_stmt->scattering->m[(dst_beta->size-1) * 2][src_stmt->scattering->nb_columns-1]);
    }

    clay_array_free(dst_beta);
}

bool substrate_is_legal_to_move_statement(
        struct osl_scop * scop,
        struct osl_statement *src_stmt,
        struct osl_statement *dst_stmt,
        bool before)
{
    bool res = false;
    struct osl_scop * candl_scop = NULL;
    struct osl_dependence *dependences = NULL;
    struct candl_options *options = NULL;
    struct candl_violation *violations = NULL;


    candl_scop = osl_scop_clone(scop);
    substrate_move_statement_scattering(candl_scop, src_stmt, dst_stmt, before);

    options = candl_options_malloc();
    candl_scop_usr_init(candl_scop);
    candl_dependence_add_extension(candl_scop, options);
    dependences = osl_generic_lookup(candl_scop->extension, OSL_URI_DEPENDENCE);
    violations = candl_violation(scop, dependences, candl_scop, options);

    if(violations == NULL)
        res = true;

    candl_options_free(options);
    candl_scop_usr_cleanup(candl_scop);
    osl_scop_free(candl_scop);
    candl_violation_free(violations);

    return res;
}


void substrate_update_adj_matrix(
        struct osl_scop * profiled_scop,
        struct substrate_adj_matrix * adj_mat,
        unsigned int stmt_index)
{
    struct osl_statement *stmt = NULL, *stmt2 = NULL;
    struct substrate_statement_profile *stmt_profile = NULL, *stmt_profile2 = NULL;
    unsigned int stmt_index2 = 0;
    unsigned int nb_stmts = 0;
    double similarity_rate = 0.0;
    bool same_domain = false;
    
    
    substrate_adj_matrix_remove_vertex(adj_mat, stmt_index);
    nb_stmts = osl_statement_number(profiled_scop->statement);
    
    stmt = profiled_scop->statement;
    for(unsigned int i = 0 ; i<nb_stmts ; i++)
    {
        if(stmt_index == i)
            break;
        stmt = stmt->next;
    }

    stmt2 = profiled_scop->statement;
    stmt_index2 = 0;
    while(stmt2 != NULL)
    {
        if(stmt == stmt2)
            continue;

        same_domain = substrate_osl_relation_equal(stmt->domain, stmt2->domain);
        if(same_domain)
        {
            stmt_profile = (struct substrate_statement_profile*) stmt->usr;
            stmt_profile2 = (struct substrate_statement_profile*) stmt2->usr;
            similarity_rate = substrate_similarity_rate(stmt_profile, stmt_profile2);

            substrate_adj_matrix_set_edge(
                    adj_mat,
                    stmt_index,
                    stmt_index2,
                    similarity_rate);
        }
        stmt2 = stmt2->next;
        stmt_index2++;
    }
}


/**
 * @brief Optimize a scop by aggregating (if possible and preferable)
 * statements of a scop with a greedy algorithm
 *
 * WIP, do not use.
 *
 * @param[inout] profiled_scop A scop profile that will be optimized.
 */
void substrate_greedy_graph_optimization(struct osl_scop * profiled_scop)
{
    struct osl_statement 
        *before_stmt1 = NULL,
        *stmt1 = NULL,
        *before_stmt2 = NULL,
        *stmt2 = NULL;
    bool same_domain = false;
    struct substrate_statement_profile *stmt_profile1 = NULL, *stmt_profile2 = NULL;
    double similarity_rate = 0.0;
    unsigned int stmt_index1 = 0, stmt_index2 = 0;
    struct substrate_adj_matrix * adj_mat = NULL;


    //if there isn't even 2 statements, we can't aggregate anything, so we return
    if((profiled_scop->statement == NULL) || (profiled_scop->statement->next == NULL))
        return;

    adj_mat = substrate_adj_matrix_new(osl_statement_number(profiled_scop->statement));

    stmt1 = profiled_scop->statement;
    stmt_index1 = 0;
    while(stmt1 != NULL)
    {
        stmt2 = stmt1->next;
        stmt_index2 = stmt_index1 + 1;
        while(stmt2 != NULL)
        {
            same_domain = substrate_osl_relation_equal(stmt1->domain, stmt2->domain);
            if(same_domain)
            {
                stmt_profile1 = (struct substrate_statement_profile*) stmt1->usr;
                stmt_profile2 = (struct substrate_statement_profile*) stmt2->usr;
                similarity_rate = substrate_similarity_rate(stmt_profile1, stmt_profile2);

                substrate_adj_matrix_set_edge(
                        adj_mat,
                        stmt_index1,
                        stmt_index2,
                        similarity_rate);
            }
            stmt_index2++;
            stmt2 = stmt2->next;
        }

        stmt_index1++;
        stmt1 = stmt1->next;
    }

    while(true)
    {
        substrate_adj_matrix_max_edge(
                adj_mat,
                &similarity_rate,
                &stmt_index1, &stmt_index2);

        if(similarity_rate < g_substrate_options.minimal_rate)
            break;

        stmt1 = substrate_get_statement(profiled_scop->statement, stmt_index1, &before_stmt1);
        stmt2 = substrate_get_statement(profiled_scop->statement, stmt_index2, &before_stmt2);
        
        if(substrate_is_legal_to_move_statement(profiled_scop, stmt1, stmt2, true))
        {
            //move stmt1 to before stmt2 and fuse stmt1 and stmt2
            substrate_move_statement_scattering(profiled_scop, stmt1, stmt2, true);
            substrate_statement_fusion_and_replace(
                    profiled_scop,
                    before_stmt1,
                    stmt1,
                    before_stmt2,
                    stmt2,
                    before_stmt2);

            substrate_update_adj_matrix(profiled_scop, adj_mat, stmt_index1);
        }
        else if(substrate_is_legal_to_move_statement(profiled_scop, stmt2, stmt1, false))
        {
            //move stmt2 to after stmt1 and fuse stmt2 and stmt1
            substrate_move_statement_scattering(profiled_scop, stmt2, stmt1, false);
            substrate_statement_fusion_and_replace(
                    profiled_scop,
                    before_stmt1,
                    stmt1,
                    before_stmt2,
                    stmt2,
                    before_stmt1);

            substrate_update_adj_matrix(profiled_scop, adj_mat, stmt_index2);
        }
        else
        {
            substrate_adj_matrix_set_edge(adj_mat, stmt_index1, stmt_index2, 0.0);
        }
    }
}
