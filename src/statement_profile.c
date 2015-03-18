#include <stdlib.h>

#include "statement_profile.h"


void substrate_scop_profile_free(
        struct substrate_scop_profile * sp)
{
    unsigned int i = 0;

    for(i=0; i<sp->size ; i++)
    {
        substrate_statement_profile_free(&sp->statement_profiles[i]);
    }
    free(sp->statement_profiles);
    sp->statement_profiles = NULL;
    sp->size = 0;
}

void substrate_statement_profile_free(
        struct substrate_statement_profile * sp)
{
    osl_statement_free(sp->osl_statement);
    sp->osl_statement = NULL;
    substrate_reuse_profile_free(&sp->reuse);
}
