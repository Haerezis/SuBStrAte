#ifndef __SUBSTRATE__REUSE_H
#define __SUBSTRATE__REUSE_H

#include <osl/osl.h>
#include "reuse_struct.h"

//génère le reuse_profile : compte et dénombre le nombre d'array différent
//et appelle generate_array_profile pour chaque array
struct substrate_reuse_profile substrate_generate_reuse_profile(
       struct osl_statement * statement);

//génère l'array_profile : compte et dénombres tout les H différents
//et appelle generate_uniformly_generated_set pour chaque
struct substrate_array_profile substrate_generate_array_profile(
        struct osl_statement * statement,
        unsigned int array_index);

//génère l'uniformly_generated_set : compte le nombre de classes et groupe les
//références dans leurs classes
struct substrate_uniformly_generated_set substrate_generate_uniformly_generated_set(
        struct osl_statement * statement,
        unsigned int array_index,
        struct osl_relation * H_matrix);

//génère le reuse_space à partir de la classe d'equivalence
struct substrate_equivalence_class substrate_generate_reuse_space(
        struct substrate_equivalence_class * ec); 

#endif
