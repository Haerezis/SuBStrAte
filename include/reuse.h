#ifndef __SUBSTRATE__REUSE_H
#define __SUBSTRATE__REUSE_H

#include <osl/osl.h>
#include "reuse_struct.h"

/**
 * A structure containing the informations needed to compute the reuse rate.
 */
struct reuse_rate
{
    unsigned int nb_same_class_refs;    /**<The number of references that can be fused in a same class */
    unsigned int nb_total_refs;         /**<The total number of references */
};


struct substrate_reuse_profile substrate_reuse_profile_constructor(
       struct osl_statement * statement);

struct substrate_array_profile substrate_array_profile_constructor(
        struct osl_relation_list * array_access_rel_list);

struct substrate_uniformly_generated_set substrate_uniformly_gen_set_constructor(
        struct osl_relation_list * uniformly_generated_set);


struct substrate_equivalence_class substrate_equivalence_class_constructor(
        struct osl_relation_list * array_references);


double substrate_rate_reuse_profiles(
        struct substrate_reuse_profile rp1,
        struct substrate_reuse_profile rp2);

struct reuse_rate substrate_rate_array_profiles(
        struct substrate_array_profile ap1,
        struct substrate_array_profile ap2);

struct reuse_rate substrate_rate_uniformly_generated_sets(
        struct substrate_uniformly_generated_set ugs1,
        struct substrate_uniformly_generated_set ugs2);

#endif
