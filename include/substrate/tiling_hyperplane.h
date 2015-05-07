#ifndef __SUBSTRATE__TILING_HYPERPLANE_H
#define __SUBSTRATE__TILING_HYPERPLANE_H

#include "tiling_hyperplane_struct.h"
#include "osl/osl.h"

struct substrate_tiling_hyperplane_profile substrate_tiling_hyperplane_profile_constructor(
       struct osl_scop * scop,
       struct osl_statement * statement);

double substrate_rate_tiling_hyperplane_profiles(
        struct substrate_tiling_hyperplane_profile thp1,
        struct substrate_tiling_hyperplane_profile thp2);

#endif
