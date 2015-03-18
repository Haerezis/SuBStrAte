#include <stdlib.h>

#include "statement_profile.h"


struct substrate_statement_profile substrate_aggregate_statement_profiles(
        struct substrate_statement_profile stmt1,
        struct substrate_statement_profile stmt2)
{
    struct substrate_statement_profile res;

    res.osl_statement = substrate_aggregate_osl_statements(
            stmt1.osl_statement,
            stmt2.osl_statement);
    
    res.reuse = substrate_reuse_profile_add(stmt1.reuse, stmt2.reuse);
    
    return res;
}

struct osl_statement * substrate_aggregate_osl_statements(
        struct osl_statement * stmt1,
        struct osl_statement * stmt2)
{
    struct osl_statement * res = NULL;
    //For now, the two statement MUST have the same domain & scattering
    //TODO : allow aggregation of statement with diff domain (→replace the clone
    //of the domain [and scattering]).
    res = osl_statement_malloc();
    res->domain = osl_relation_clone(stmt1->domain);
    res->scattering = osl_relation_clone(stmt1->scattering);
    res->access = NULL;
    osl_relation_list_add(
            &res->access,
            osl_relation_list_clone(stmt1->access));
    osl_relation_list_add(
            &res->access,
            osl_relation_list_clone(stmt1->access));
    res->extension = NULL;
    osl_generic_add(
            &res->extension,
            stmt1->extension);
    osl_generic_add(
            &res->extension,
            stmt2->extension);
    res->usr = NULL;
    res->next = stmt2->next;
    
    return res;
}

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
