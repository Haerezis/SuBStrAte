#ifndef __SUBSTRATE__UTILS_H
#define __SUBSTRATE__UTILS_H

#include <osl/osl.h>

/**
 * A structure representing a groups of relations.
 */
struct substrate_osl_relation_group_list {
    struct osl_relation_list ** list;   /**<An array of pointer that point to the first relation of each relation group */
    unsigned int size;                  /**<The number of group in the array "list" */
    unsigned int max_size;              /**<The max size of the array */
};


osl_int_t substrate_get_array_id_from_access_relation(
        struct osl_relation * rel);


struct substrate_osl_relation_group_list substrate_group_access_relations_by(
        struct osl_relation_list * rl,
        bool (*by_function) (struct osl_relation *,struct osl_relation *));

struct osl_scop * substrate_osl_scop_nclone_expect_statement(
        struct osl_scop * scop,
        int n);


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



bool substrate_same_scattering_and_beta_depth(
        struct osl_relation * rel1,
        struct osl_relation * rel2);

void substrate_reset_beta_depth(
        struct osl_statement * stmt);



struct osl_generic * substrate_osl_generic_lookup(
        struct osl_generic * x,
        char const * const URI);
struct osl_generic * substrate_osl_generic_nclone(
        struct osl_generic * generic,
        int n);
void substrate_osl_strings_concat(
        struct osl_strings ** dest,
        struct osl_strings * str1,
        struct osl_strings * str2);
#endif
