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

#endif
