#ifndef __SUBSTRATE__OPTIMIZATION_H
#define __SUBSTRATE__OPTIMIZATION_H

#include <osl/osl.h>

#include "statement_profile.h"

struct osl_scop * substrate_optimize(
        struct substrate_scop_profile scop_profile);


#endif
