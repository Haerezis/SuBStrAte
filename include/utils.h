#ifndef __SUBSTRATE__UTILS_H
#define __SUBSTRATE__UTILS_H

#include <osl/osl.h>

struct substrate_osl_relation_list_set {
    struct osl_relation_list ** set;
    unsigned int size;
    unsigned int max_size;
};


osl_int_t substrate_get_array_id_from_access_relation(
        struct osl_relation * rel);

struct substrate_osl_relation_list_set substrate_group_access_relations_by_array(
        struct osl_relation_list * rl);


struct substrate_osl_relation_list_set substrate_group_access_relations_by_H_matrix(
        struct osl_relation_list * rl);


bool substrate_H_matrix_eq(
        struct osl_relation * access_relation1,
        struct osl_relation * access_relation2);

bool substrate_H_matrix_neq(
        struct osl_relation * access_relation1,
        struct osl_relation * access_relation2);

#endif
