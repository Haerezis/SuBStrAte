#include "optimization.h"


struct osl_scop * substrate_optimize(
        struct substrate_statement_profile * statement_profiles,
        struct osl_scop * scope)
{
    return osl_scop_clone(scope);
}
