#include "optimization.h"


struct osl_scop * substrate_optimize(
        struct substrate_scop_profile scop_profile)
{
    return osl_scop_clone(scop_profile.scop);
}
