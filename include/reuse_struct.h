#ifndef __SUBSTRATE__REUSE_STRUCT_H
#define __SUBSTRATE__REUSE_STRUCT_H

#include <osl/osl.h>


//depecrated structure
////////////////////////////////////////////////
struct substrate_reuse_space
{
    unsigned int * spanning_vector_indexes;
    unsigned int size;
};
////////////////////////////////////////////////




struct substrate_equivalence_class
{
    struct osl_relation_list * array_references;
    unsigned int size;
//    struct substrate_reuse_space reuse_space;
};

struct substrate_uniformly_generated_set
{
    struct osl_relation * H_matrix;

    struct substrate_equivalence_class * classes;
    unsigned int size;
};

struct substrate_array_profile
{
    osl_int_t array_id;
    struct substrate_uniformly_generated_set * uniformly_gen_sets;
    unsigned int size;
};

struct substrate_reuse_profile
{
    struct osl_statement * osl_statement;
    struct substrate_array_profile * array_profiles;
    unsigned int size;
};



void substrate_reuse_profile_free(
        struct substrate_reuse_profile * rp);
void substrate_array_profile_free(
        struct substrate_array_profile * ap);
void substrate_uniformly_generated_set_free(
        struct substrate_uniformly_generated_set * ugs);
void substrate_equivalence_class_free(
        struct substrate_equivalence_class * ec);
void substrate_reuse_space_free(
        struct substrate_reuse_space * rp);


void substrate_reuse_profile_dump(
        FILE * output_stream,
        struct substrate_reuse_profile * rp);
void substrate_array_profile_dump(
        FILE * output_stream,
        struct substrate_array_profile * ap);
void substrate_uniformly_generated_set_dump(
        FILE * output_stream,
        struct substrate_uniformly_generated_set * ugs);
void substrate_equivalence_class_dump(
        FILE * output_stream,
        struct substrate_equivalence_class * ec);
#endif
