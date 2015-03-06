#ifndef __SUBSTRATE__OPTIMIZATION_H
#define __SUBSTRATE__OPTIMIZATION_H

#include <osl/osl.h>

#include "statement_profile.h"

struct osl_scop * substrate_optimize(
        struct substrate_statement_profile * statement_profiles,
        struct osl_scop * scope);


#endif
