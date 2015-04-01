#ifndef __SUBSTRATE__STATEMENT_PROFILE_H
#define __SUBSTRATE__STATEMENT_PROFILE_H

#include <osl/osl.h>

#include "reuse.h"
#include "parallelism.h"

/**
 * A structure representing the profile of a statement.
 * It should contain the reuse, vectorize, parallel ... profile of the statement.
 */
struct substrate_statement_profile
{
    struct substrate_reuse_profile reuse;   /**<The reuse profile of the stagtement. */
    struct substrate_parallelism_profile parallelism;   /**<The parallelism profile of the stagtement. */
};


struct substrate_statement_profile * substrate_statement_profile_malloc();
struct substrate_statement_profile * substrate_statement_profile_constructor(
        struct osl_statement *);
struct osl_scop * substrate_osl_scop_clone(struct osl_scop * scop);
struct substrate_statement_profile * substrate_statement_profile_clone(
        struct substrate_statement_profile * stmt_profile);


struct osl_statement * substrate_statement_fusion(
        struct osl_statement * stmt1,
        struct osl_statement * stmt2);
struct substrate_statement_profile * substrate_statement_profile_fusion(
        struct substrate_statement_profile * stmt1,
        struct substrate_statement_profile * stmt2);
struct osl_statement * substrate_osl_statement_fusion(
        struct osl_statement * stmt1,
        struct osl_statement * stmt2);
struct osl_generic * substrate_osl_generic_fusion(
        struct osl_generic * gen1,
        struct osl_generic * gen2);
struct osl_body * substrate_osl_body_fusion(
        struct osl_body * body1,
        struct osl_body * body2);


void substrate_scop_profile_free(
        struct osl_scop * sp);

void substrate_statement_profile_free(
        struct substrate_statement_profile * sp);

#endif
