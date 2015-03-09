#ifndef __SUBSTRATE__REUSE_STRUCT_H
#define __SUBSTRATE__REUSE_STRUCT_H


struct substrate_reuse_space
{
    unsigned int * vector_indexes;
    unsigned int size;
    unsigned int max_size;
};

struct substrate_equivalence_class
{
    unsigned int * array_reference_indexes;
    unsigned int size;
    unsigned int max_size;
    struct substrate_reuse_space reuse_space;
};

struct substrate_uniformly_generated_set
{
    struct substrate_equivalence_class * temporal_classes;
    unsigned int temporal_size;
    unsigned int temporal_max_size;

    struct substrate_equivalence_class * spatial_classes;
    unsigned int spatial_size;
    unsigned int spatial_max_size;
};

struct substrate_array_profile
{
    unsigned int array_id;
    struct substrate_uniformly_generated_set * uniformly_gen_sets;
    unsigned int size;
    unsigned int max_size;
};

struct substrate_reuse_profile
{
    struct osl_statement * osl_statement;
    struct substrate_array_profile * array_profiles;
    unsigned int size;
    unsigned int max_size;
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

#endif
