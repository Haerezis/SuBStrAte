#ifndef __SUBSTRATE__STATEMENT_PROFILE_H
#define __SUBSTRATE__STATEMENT_PROFILE_H

#include <osl/osl.h>

#include "reuse.h"

struct substrate_statement_profile
{
    struct osl_statement *osl_informations;
    struct substrate_reuse_profile * reuse;
};

#endif
