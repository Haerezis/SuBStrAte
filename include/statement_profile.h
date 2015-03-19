#ifndef __SUBSTRATE__STATEMENT_PROFILE_H
#define __SUBSTRATE__STATEMENT_PROFILE_H

#include <osl/osl.h>

#include "reuse.h"

struct substrate_scop_profile
{
    struct osl_scop * scop;
    struct substrate_statement_profile * statement_profiles;
    unsigned int size;
};

struct substrate_statement_profile
{
    struct osl_statement * osl_statement;
    struct substrate_reuse_profile reuse;
};




struct substrate_statement_profile substrate_statement_profile_fusion(
        struct substrate_statement_profile stmt1,
        struct substrate_statement_profile stmt2);
struct osl_statement * substrate_osl_statement_fusion(
        struct osl_statement * stmt1,
        struct osl_statement * stmt2);

struct osl_scop * substrate_scop_profile_to_osl_scop(struct substrate_scop_profile scop_profile);

void substrate_scop_profile_free(
        struct substrate_scop_profile * sp);

void substrate_statement_profile_free(
        struct substrate_statement_profile * sp);

#endif
