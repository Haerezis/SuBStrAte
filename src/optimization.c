#include <stdlib.h>
#include <string.h>

#include "optimization.h"

#include "options.h"
#include "reuse.h"

/**
 * dest := source (except source->statement)
 */
void substrate_copy_scop_except_statements(
        struct osl_scop * dest,
        struct osl_scop * source)
{
    dest->version = source->version;
    dest->language = malloc(strlen(source->language)+1);//alloc the string
    strcpy(dest->language,source->language);//copy the string after alloc
    dest->context = osl_relation_clone(source->context);
    dest->parameters = osl_generic_clone(source->parameters);
    dest->statement = NULL;
    dest->registry = osl_interface_clone(source->registry);
    dest->extension = osl_generic_clone(source->extension);
    dest->usr = NULL;
    dest->next = NULL;//TODO : substrate doesn't handle files with multiples scope for now
}

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
