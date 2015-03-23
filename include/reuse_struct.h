#ifndef __SUBSTRATE__REUSE_STRUCT_H
#define __SUBSTRATE__REUSE_STRUCT_H

#include <osl/osl.h>


/**
 * A structure representing an equivalence class.
 */
struct substrate_equivalence_class
{
    struct osl_relation_list * array_references;    /**<An osl_relation_list representing the class*/
    unsigned int size;                              /**<The number of relation in the class */
};

/**
 * A structure representing a Uniformly Generated Set.
 * A Uniformly Generated Set is a set of reference sharing the same H matrix (see Wolf91).
 * The references in the set are also classify in equivalence classes.
 */
struct substrate_uniformly_generated_set
{
    struct osl_relation * H_matrix;                 /**<A relation containing the H matrix shared by all the references */

    struct substrate_equivalence_class * classes;   /**<An array of substrate_equivalence_class for each equivalence class*/
    unsigned int size;                              /**<The number of equivalence class in "classes" */
};

/**
 * A structure reprenting an array profile.
 * A array profile is caracterize by an array id (all references in an array profile share
 * the same array id), and a certain number of Uniformly Generated Sets, depending on the
 * number of different H matrix in the references.
 */
struct substrate_array_profile
{
    osl_int_t array_id;                         /**<The array id */
    int array_id_precision;                     /**<The precision of the array id */
    struct substrate_uniformly_generated_set
        * uniformly_gen_sets;                   /**<An array of Uniformly Generated Set */
    unsigned int size;                          /**<The number of Uniformly Generated Set in the array */
};

/**
 * A structure representing a reuse profile.
 * A reuse profile is simply a list of array profile, one for each different array 
 * (different as in the name/id of the array) in the profile.
 */
struct substrate_reuse_profile
{
    struct substrate_array_profile * array_profiles;    /**<An array of array profile */
    unsigned int size;                                  /**<The number of array profile in the array of array profile (That's a lot of array...) */
};


struct substrate_reuse_profile substrate_reuse_profile_clone(
        struct substrate_reuse_profile rp);
struct substrate_array_profile substrate_array_profile_clone(
        struct substrate_array_profile ap);
struct substrate_uniformly_generated_set substrate_uniformly_generated_set_clone(
        struct substrate_uniformly_generated_set ugs);
struct substrate_equivalence_class substrate_equivalence_class_clone(
        struct substrate_equivalence_class ec);

void substrate_reuse_profile_free(
        struct substrate_reuse_profile * rp);
void substrate_array_profile_free(
        struct substrate_array_profile * ap);
void substrate_uniformly_generated_set_free(
        struct substrate_uniformly_generated_set * ugs);
void substrate_equivalence_class_free(
        struct substrate_equivalence_class * ec);


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


struct substrate_reuse_profile substrate_reuse_profile_fusion(
        struct substrate_reuse_profile rp1,
        struct substrate_reuse_profile rp2);
struct substrate_array_profile substrate_array_profile_fusion(
        struct substrate_array_profile ap1,
        struct substrate_array_profile ap2);
struct substrate_uniformly_generated_set substrate_uniformly_generated_set_fusion(
        struct substrate_uniformly_generated_set ugs1,
        struct substrate_uniformly_generated_set ugs2);
struct substrate_equivalence_class substrate_equivalence_class_fusion(
        struct substrate_equivalence_class ec1,
        struct substrate_equivalence_class ec2);


unsigned int substrate_uniformly_generated_set_count_access(
        struct substrate_uniformly_generated_set ugs);
unsigned int substrate_array_profile_count_access(
        struct substrate_array_profile ap);
#endif
