#ifndef __SUBSTRATE__PARALLELISM_H
#define __SUBSTRATE__PARALLELISM_H

#include "parallelism_struct.h"
#include "osl/osl.h"

struct substrate_parallelism_profile substrate_parallelism_profile_constructor(
       struct osl_statement * statement,
       struct osl_scop * scop);

double substrate_rate_parallelism_profiles(
        struct substrate_parallelism_profile pp1,
        struct substrate_parallelism_profile pp2);

#endif
