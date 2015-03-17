#ifndef __SUBSTRATE__REUSE_H
#define __SUBSTRATE__REUSE_H

#include <osl/osl.h>
#include "reuse_struct.h"

//génère le reuse_profile : compte et dénombre le nombre d'array différent
//et appelle generate_array_profile pour chaque array
struct substrate_reuse_profile substrate_reuse_profile_constructor(
       struct osl_statement * statement);

//génère l'array_profile : compte et dénombres tout les H différents
//et appelle generate_uniformly_generated_set pour chaque
struct substrate_array_profile substrate_array_profile_constructor(
        struct osl_relation_list * array_access_rel_list);

//génère l'uniformly_generated_set : compte le nombre de classes et groupe les
//références dans leurs classes
struct substrate_uniformly_generated_set substrate_uniformly_gen_set_constructor(
        struct osl_relation_list * uniformly_generated_set);


struct substrate_equivalence_class substrate_equivalence_class_constructor(
        struct osl_relation_list * array_references);


double substrate_rate_reuse_profiles(
        struct substrate_reuse_profile rp1,
        struct substrate_reuse_profile rp2);

double substrate_rate_array_profiles(
        struct substrate_array_profile ap1,
        struct substrate_array_profile ap2);

double substrate_rate_uniformly_generated_sets(
        struct substrate_uniformly_generated_set ugs1,
        struct substrate_uniformly_generated_set ugs2);

#endif
