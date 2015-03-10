#ifndef __SUBSTRATE__UTILS_H
#define __SUBSTRATE__UTILS_H

#include <osl/osl.h>

struct substrate_osl_int_set {
    osl_int_t * set;
    unsigned int size;
    unsigned int max_size;
};


struct substrate_osl_int_set substrate_array_id_set_from_access_relations(
        struct osl_relation_list * rl);

#endif
