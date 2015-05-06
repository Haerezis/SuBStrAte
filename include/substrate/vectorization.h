#ifndef __SUBSTRATE__VECTORIZATION_H
#define __SUBSTRATE__VECTORIZATION_H

#include "substrate/vectorization_struct.h"
#include "osl/osl.h"

struct substrate_vectorization_profile substrate_vectorization_profile_constructor(
       struct osl_scop * scop,
       struct osl_statement * statement);

double substrate_rate_vectorization_profiles(
        struct substrate_vectorization_profile vp1,
        struct substrate_vectorization_profile vp2);

#endif
