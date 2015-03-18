#ifndef __SUBSTRATE__UTILS_H
#define __SUBSTRATE__UTILS_H

#include <osl/osl.h>

struct substrate_osl_relation_group_list {
    struct osl_relation_list ** list;
    unsigned int size;
    unsigned int max_size;
};


osl_int_t substrate_get_array_id_from_access_relation(
        struct osl_relation * rel);


struct substrate_osl_relation_group_list substrate_group_access_relations_by(
        struct osl_relation_list * rl,
        bool (*by_function) (struct osl_relation *,struct osl_relation *));


bool substrate_array_id_eq(
        struct osl_relation * access_relation1,
        struct osl_relation * access_relation2);

bool substrate_H_matrix_eq(
        struct osl_relation * access_relation1,
        struct osl_relation * access_relation2);

bool substrate_H_matrix_neq(
        struct osl_relation * access_relation1,
        struct osl_relation * access_relation2);

bool substrate_access_class_eq(
        struct osl_relation * access_relation1,
        struct osl_relation * access_relation2);

bool substrate_spatial_class_eq(
        struct osl_relation * access_relation1,
        struct osl_relation * access_relation2);

bool substrate_temporal_class_eq(
        struct osl_relation * access_relation1,
        struct osl_relation * access_relation2);


void substrate_copy_scop_except_statements(
        struct osl_scop * source,
        struct osl_scop * destination);

#endif
