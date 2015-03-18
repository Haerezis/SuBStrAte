#include <stdlib.h>

#include "optimization.h"

#include "options.h"
#include "reuse.h"
#include "utils.h"


struct osl_scop * substrate_optimize(
        struct substrate_scop_profile scop_profile)
{
    struct osl_scop * scop = NULL;

    scop = osl_scop_malloc();
    substrate_copy_scop_except_statements(scop,scop_profile.scop);

    substrate_successive_statements_optimization(scop, scop_profile);

    return scop;
}



void substrate_successive_statements_optimization(
        struct osl_scop * scop,
        struct substrate_scop_profile scop_profile)
{
    unsigned int i = 0;
    double reuse_rate = 0.0;

    for(i=0 ; i<(scop_profile.size-1) ; i++)
    {
        reuse_rate = substrate_rate_reuse_profiles(
                scop_profile.statement_profiles[i].reuse,
                scop_profile.statement_profiles[i+1].reuse);
        if(reuse_rate >= g_substrate_options.minimal_reuse_rate)
        {
            //TODO
        }
    }
}
