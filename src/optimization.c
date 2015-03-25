#include <stdlib.h>

#include "optimization.h"

#include "options.h"
#include "reuse.h"
#include "utils.h"
#include "osl/osl.h"

/**
 * @brief Optimize a scop by aggregating similar behaving statements, using the
 * statements profiles contained in \a scop_profile.
 *
 * @param[in] scop_profile A scop profile (containing statement profiles)
 * that will be optimized.
 *
 * @return an osl_scop corresponding to optimization of \a scop_profile.
 */
struct osl_scop * substrate_optimize(
        struct substrate_scop_profile scop_profile)
{
    struct osl_scop * scop = NULL;

    substrate_successive_statements_optimization(&scop_profile);
    scop = substrate_scop_profile_to_osl_scop(scop_profile);

    return scop;
}




/**
 * @brief Optimize a scop by aggregating (if possible and preferable)
 * successive statements one by one.
 *
 * @param[inout] scop_profile A scop profile that will be optimized.
 */
void substrate_successive_statements_optimization(struct substrate_scop_profile * scop_profile)
{
    unsigned int i1 = 0, i2 = 0;
    struct substrate_statement_profile stmt_profile;
    bool same_domain = false, same_scattering = false;
    double reuse_rate = 0.0;


    for(i2=1 ; i2<(scop_profile->size) ; i2++)
    {
        same_domain = osl_relation_equal(
                scop_profile->statement_profiles[i1].osl_statement->domain,
                scop_profile->statement_profiles[i2].osl_statement->domain);

        if(same_domain == true)
        {
            same_scattering = substrate_same_scattering_and_beta_depth(
                    scop_profile->statement_profiles[i1].osl_statement->scattering,
                    scop_profile->statement_profiles[i2].osl_statement->scattering);

            if(same_scattering == true)
            {
                reuse_rate = substrate_rate_reuse_profiles(
                        scop_profile->statement_profiles[i1].reuse,
                        scop_profile->statement_profiles[i2].reuse);
                if(reuse_rate >= g_substrate_options.minimal_reuse_rate)
                {
                    stmt_profile = substrate_statement_profile_fusion(
                            scop_profile->statement_profiles[i1],
                            scop_profile->statement_profiles[i2]);
                    substrate_statement_profile_free(&scop_profile->statement_profiles[i1]);
                    substrate_statement_profile_free(&scop_profile->statement_profiles[i2]);
                    scop_profile->statement_profiles[i1] = stmt_profile;
                }
                else
                {
                    i1++;
                    scop_profile->statement_profiles[i1] = scop_profile->statement_profiles[i2];
                }
            }
            else
            {
                //TODO Use candl and simulate a move of the 2nd statement at the same
                //scattering and beta depth than the 1st (using the base and modified scattering)
            }
        }
        else
        {
            i1++;
            scop_profile->statement_profiles[i1] = scop_profile->statement_profiles[i2];
        }
    }

}
