#ifndef __SUBSTRATE__STATEMENT_PROFILE_H
#define __SUBSTRATE__STATEMENT_PROFILE_H

#include <osl/osl.h>

#include "reuse.h"

/**
 * A structure representing a scop profile.
 * Basically it's just an array of statement_profile.
 */
struct substrate_scop_profile
{
    struct osl_scop * scop;                                 /**<A pointer to the osl_scop of the profile */
    struct substrate_statement_profile * statement_profiles;/**<An array of statement_profile */
    unsigned int size;                                      /**<The number of statement_profile in the array */
};

/**
 * A structure representing the profile of a statement.
 * It should contain the reuse, vectorize, parallel ... profile of the statement.
 */
struct substrate_statement_profile
{
    struct osl_statement * osl_statement;   /**<A pointer to the osl_statement of this profile */
    struct substrate_reuse_profile reuse;   /**<The reuse profile of the stagtement. */
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
