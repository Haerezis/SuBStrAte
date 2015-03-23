#include <stdlib.h>

#include "osl/osl.h"

#include "reuse_struct.h"
#include "utils.h"

/**
 * @brief Create a recursive copy (not just a pointer copy) of a substrate_reuse_profile.
 *
 * @param[in] rp The substrate_reuse_profile to be copied.
 *
 * @return A substrate_reuse_profile that is the clone of \a rp. 
 */
struct substrate_reuse_profile substrate_reuse_profile_clone(
        struct substrate_reuse_profile rp)
{
    unsigned int i = 0;
    struct substrate_reuse_profile res;

    res.array_profiles = (struct substrate_array_profile*)
        malloc(rp.size * sizeof(struct substrate_array_profile));
    res.size = rp.size;
    for(i=0 ; i<res.size ; i++)
    {
        res.array_profiles[i] = substrate_array_profile_clone(rp.array_profiles[i]);
    }

    return res;
}


/**
 * @brief Create a recursive copy (not just a pointer copy) of a substrate_array_profile.
 *
 * @param[in] ap The substrate_array_profile to be copied.
 *
 * @return A substrate_array_profile that is the clone of \a ap. 
 */
struct substrate_array_profile substrate_array_profile_clone(
        struct substrate_array_profile ap)
{
    unsigned int i = 0;
    struct substrate_array_profile res;
   
    
    res.array_id_precision = ap.array_id_precision;
    osl_int_init_set(res.array_id_precision, &res.array_id, ap.array_id);
    res.uniformly_gen_sets = (struct substrate_uniformly_generated_set*)
        malloc(ap.size * sizeof(struct substrate_uniformly_generated_set));
    res.size = ap.size;
    for(i=0 ; i<res.size ; i++)
    {
        res.uniformly_gen_sets[i] = substrate_uniformly_generated_set_clone(
                ap.uniformly_gen_sets[i]);
    }

    return res;
}


/**
 * @brief Create a recursive copy (not just a pointer copy) of a substrate_uniformly_generated_set.
 *
 * @param[in] ugs The substrate_uniformly_generated_set to be copied.
 *
 * @return A substrate_uniformly_generated_set that is the clone of \a ugs. 
 */
struct substrate_uniformly_generated_set substrate_uniformly_generated_set_clone(
        struct substrate_uniformly_generated_set ugs)
{
    unsigned int i = 0;
    struct substrate_uniformly_generated_set res;

    res.H_matrix = osl_relation_clone(ugs.H_matrix);
    res.classes = (struct substrate_equivalence_class*)
        malloc(ugs.size * sizeof(struct substrate_equivalence_class));
    res.size = ugs.size;
    for(i=0 ; i<res.size ; i++)
    {
        res.classes[i] = substrate_equivalence_class_clone(ugs.classes[i]);
    }

    return res;
}


/**
 * @brief Create a recursive copy (not just a pointer copy) of a substrate_equivalence_class.
 *
 * @param[in] ec The substrate_equivalence_class to be copied.
 *
 * @return A substrate_equivalence_class that is the clone of \a ec. 
 */
struct substrate_equivalence_class substrate_equivalence_class_clone(
        struct substrate_equivalence_class ec)
{
    struct substrate_equivalence_class res;

    res.array_references = osl_relation_list_clone(ec.array_references);
    res.size = ec.size;

    return res;
}


/**
 * @brief Free recursively a substrate_reuse_profile.
 *
 * @param[in] rp The substrate_reuse_profile that will be freed.
 */
void substrate_reuse_profile_free(
        struct substrate_reuse_profile * rp)
{
    unsigned int i = 0;
    for(i=0; i < rp->size ; i++)
        substrate_array_profile_free(&rp->array_profiles[i]);
    free(rp->array_profiles);
    rp->array_profiles = NULL;
    rp->size = 0;
}

/**
 * @brief Free recursively a substrate_array_profile.
 *
 * @param[in] ap The substrate_array_profile that will be freed.
 */
void substrate_array_profile_free(
        struct substrate_array_profile * ap)
{
    unsigned int i = 0;
    for(i=0 ; i < ap->size ; i++)
        substrate_uniformly_generated_set_free(&ap->uniformly_gen_sets[i]);
    free(ap->uniformly_gen_sets);
    ap->uniformly_gen_sets = NULL;
    ap->size = 0;

    osl_int_clear(ap->array_id_precision, &ap->array_id);
    ap->array_id_precision = 0;
}


/**
 * @brief Free recursively a substrate_uniformly_generated_set.
 *
 * @param[in] ugs The substrate_uniformly_generated_set that will be freed.
 */
void substrate_uniformly_generated_set_free(
        struct substrate_uniformly_generated_set * ugs)
{
    unsigned int i = 0;
    for(i=0 ; i< ugs->size ; i++)
        substrate_equivalence_class_free(&ugs->classes[i]);
    free(ugs->classes);
    ugs->classes = NULL;
    ugs->size = 0;
    
    ugs->H_matrix = NULL;
}

/**
 * @brief Free recursively a substrate_equivalence_class.
 *
 * @param[in] ec The substrate_equivalence_class that will be freed.
 */
void substrate_equivalence_class_free(
        struct substrate_equivalence_class * ec)
{
    osl_relation_list_free(ec->array_references);
    ec->array_references = NULL;
    ec->size = 0;
}



/**
 * @brief Print the content of a substrate_reuse_profile in an output stream
 * (file, stdout...).
 *
 * @param[in] output_stream The output stream where the content will be printed.
 * @param[in] rp The structure that will be printed.
 */
void substrate_reuse_profile_dump(
        FILE * output_stream,
        struct substrate_reuse_profile * rp)
{
    unsigned int i = 0;

    fprintf(output_stream,"Number of array in the statement : %d\n", rp->size);
    for(i=0 ; i<rp->size ; i++)
    {
        substrate_array_profile_dump(output_stream, &rp->array_profiles[i]);
    }
}


/**
 * @brief Print the content of a substrate_array_profile in an output stream
 * (file, stdout...).
 *
 * @param[in] output_stream The output stream where the content will be printed.
 * @param[in] ap The structure that will be printed.
 */
void substrate_array_profile_dump(
        FILE * output_stream,
        struct substrate_array_profile * ap)
{
    unsigned int i = 0;

    fprintf(output_stream,"\tArray id : ");
    osl_int_print(output_stream,ap->array_id_precision, ap->array_id);
    fprintf(output_stream,"\n");
    fprintf(output_stream,"\tNumber of different Uniformly Generated Sets : %d\n",ap->size);
    for(i=0; i<ap->size ; i++)
    {
        substrate_uniformly_generated_set_dump(output_stream,&ap->uniformly_gen_sets[i]);
    }
}


/**
 * @brief Print the content of a substrate_uniformly_generated_set in an output stream
 * (file, stdout...).
 *
 * @param[in] output_stream The output stream where the content will be printed.
 * @param[in] ugs The structure that will be printed.
 */
void substrate_uniformly_generated_set_dump(
        FILE * output_stream,
        struct substrate_uniformly_generated_set * ugs)
{
    unsigned int i = 0;

    fprintf(output_stream,"\t\tH matrix :\n");
    osl_relation_idump(output_stream,ugs->H_matrix,2);
    fprintf(output_stream,"\t\tNumber of equivalence classes : %d\n",ugs->size);
    fprintf(output_stream,"\t\tEquivalence classes\n");
    for(i=0; i<ugs->size ; i++)
    {
        substrate_equivalence_class_dump(output_stream,&ugs->classes[i]);
    }
}


/**
 * @brief Print the content of a substrate_equivalence_class in an output stream
 * (file, stdout...).
 *
 * @param[in] output_stream The output stream where the content will be printed.
 * @param[in] ec The structure that will be printed.
 */
void substrate_equivalence_class_dump(
        FILE * output_stream,
        struct substrate_equivalence_class * ec)
{
    fprintf(output_stream,"\n");
    fprintf(output_stream,"\t\t######################################################\n");
    fprintf(output_stream,"\t\tNumber of references in the class : %d\n",ec->size);
    fprintf(output_stream,"\n");
    osl_relation_list_idump(output_stream,ec->array_references,3);
}



/**
 * @brief Fusion recursively two substrate_reuse_profile into a third one.
 *
 * \a rp1 and \a rp2 are not modified, everything is cloned/copied into the result.
 *
 * @param[in] rp1 The first substrate_reuse_profile.
 * @param[in] rp2 The second substrate_reuse_profile.
 *
 * @return A substrate_reuse_profile, result of the fusion of \a rp1 and \a rp2 .
 */
struct substrate_reuse_profile substrate_reuse_profile_fusion(
        struct substrate_reuse_profile rp1,
        struct substrate_reuse_profile rp2)
{
    unsigned int i=0, i1 = 0, i2 = 0;
    unsigned int max_size = 0, res_size = 0;
    struct substrate_reuse_profile res;
    
    max_size = rp1.size + rp2.size;
    res.array_profiles = (struct substrate_array_profile*)
        malloc(max_size * sizeof(struct substrate_array_profile));

    for(i1=0 ; i<rp1.size ; i1++)
    {
        for(i2=0; i2<rp2.size ; i2++)
        {
            if(osl_int_eq(
                        rp1.array_profiles[i1].array_id_precision,
                        rp1.array_profiles[i1].array_id,
                        rp2.array_profiles[i2].array_id))
            {
                res.array_profiles[res.size] = 
                    substrate_array_profile_fusion(
                            rp1.array_profiles[i1],
                            rp2.array_profiles[i2]);
                res.size++;
            }
        }
        if(i2 >= rp2.size)
        {
            res.array_profiles[res.size] = 
                substrate_array_profile_clone(rp1.array_profiles[i1]);
            res.size++;
        }
    }

    res_size = res.size;
    for(i2=0 ; i2<rp2.size ; i2++)
    {
        for(i=0 ; i<res_size ; i++)
        {
            if(osl_int_eq(
                        rp1.array_profiles[i1].array_id_precision,
                        rp1.array_profiles[i1].array_id,
                        rp2.array_profiles[i2].array_id))
            {
                break;
            }
        }
        if(i >= res_size)
        {
            res.array_profiles[res.size] = 
                substrate_array_profile_clone(rp2.array_profiles[i2]);
            res.size++;
        }
    }

    return res;
}

/**
 * @brief Fusion recursively two substrate_array_profile into a third one.
 *
 * \a ap1 and \a ap2 are not modified, everything is cloned/copied into the result.
 *
 * @param[in] ap1 The first substrate_array_profile.
 * @param[in] ap2 The second substrate_array_profile.
 *
 * @return A substrate_array_profile, result of the fusion of \a ap1 and \a ap2 .
 */
struct substrate_array_profile substrate_array_profile_fusion(
        struct substrate_array_profile ap1,
        struct substrate_array_profile ap2)
{
    unsigned int i=0, i1 = 0, i2 = 0;
    unsigned int max_size = 0, res_size = 0;
    struct substrate_array_profile res;
    
    max_size = ap1.size + ap2.size;
    res.array_id_precision = ap1.array_id_precision;
    osl_int_init_set(res.array_id_precision, &res.array_id, ap1.array_id);
    res.uniformly_gen_sets = (struct substrate_uniformly_generated_set*)
        malloc(max_size * sizeof(struct substrate_uniformly_generated_set));

    for(i1=0 ; i<ap1.size ; i1++)
    {
        for(i2=0; i2<ap2.size ; i2++)
        {
            if(substrate_H_matrix_eq(
                        ap1.uniformly_gen_sets[i1].H_matrix,
                        ap2.uniformly_gen_sets[i2].H_matrix))
            {
                res.uniformly_gen_sets[res.size] = 
                    substrate_uniformly_generated_set_fusion(
                            ap1.uniformly_gen_sets[i1],
                            ap2.uniformly_gen_sets[i2]);
                res.size++;
            }
        }
        if(i2 >= ap2.size)
        {
            res.uniformly_gen_sets[res.size] = 
                substrate_uniformly_generated_set_clone(ap1.uniformly_gen_sets[i1]);
            res.size++;
        }
    }

    res_size = res.size;
    for(i2=0 ; i2<ap2.size ; i2++)
    {
        for(i=0 ; i<res_size ; i++)
        {
            if(substrate_access_class_eq(
                        res.uniformly_gen_sets[i1].H_matrix,
                        ap2.uniformly_gen_sets[i2].H_matrix))
            {
                break;
            }
        }
        if(i >= res_size)
        {
            res.uniformly_gen_sets[res.size] = 
                substrate_uniformly_generated_set_clone(ap2.uniformly_gen_sets[i2]);
            res.size++;
        }
    }

    return res;
}


/**
 * @brief Fusion recursively two substrate_uniformly_generated_set into a third one.
 *
 * \a ugs1 and \a ugs2 are not modified, everything is cloned/copied into the result.
 *
 * @param[in] ugs1 The first substrate_uniformly_generated_set.
 * @param[in] ugs2 The second substrate_uniformly_generated_set.
 *
 * @return A substrate_uniformly_generated_set, result of the fusion of \a ugs1 and \a ugs2 .
 */
struct substrate_uniformly_generated_set substrate_uniformly_generated_set_fusion(
        struct substrate_uniformly_generated_set ugs1,
        struct substrate_uniformly_generated_set ugs2)
{
    struct substrate_uniformly_generated_set res;
    unsigned int i=0, i1 = 0, i2 = 0;
    unsigned int max_size = 0, res_size = 0;

    max_size = ugs1.size + ugs2.size;
    res.H_matrix = osl_relation_clone(ugs1.H_matrix);
    res.size = 0;
    res.classes = (struct substrate_equivalence_class*)
        malloc(max_size * sizeof(struct substrate_equivalence_class));

    for(i1=0 ; i1<ugs1.size ; i1++)
    {
        for(i2=0 ; i2<ugs2.size ; i2++)
        {

            if(substrate_access_class_eq(
                        ugs1.classes[i1].array_references->elt,
                        ugs2.classes[i2].array_references->elt))
            {
                res.classes[res.size] = substrate_equivalence_class_fusion(
                        ugs1.classes[i1],
                        ugs2.classes[i2]);
                res.size++;
                break;
            }
        }
        if(i2 >= ugs2.size)
        {
            res.classes[res.size] = substrate_equivalence_class_clone(ugs1.classes[i1]);
            res.size++;
        }
    }

    res_size = res.size;
    for(i2=0 ; i2<ugs2.size ; i2++)
    {
        for(i=0 ; i<res_size ; i++)
        {
            if(substrate_access_class_eq(
                        res.classes[i].array_references->elt,
                        ugs2.classes[i2].array_references->elt))
            {
                break;
            }
        }
        if(i >= res_size)
        {
            res.classes[res.size] = substrate_equivalence_class_clone(ugs2.classes[i2]);
            res.size++;
        }
    }

    return res;
}

/**
 * @brief Fusion recursively two substrate_equivalence_class into a third one.
 *
 * \a ec1 and \a ec2 are not modified, everything is cloned/copied into the result.
 *
 * @param[in] ec1 The first substrate_equivalence_class.
 * @param[in] ec2 The second substrate_equivalence_class.
 *
 * @return A substrate_equivalence_class, result of the fusion of \a ec1 and \a ec2 .
 */
struct substrate_equivalence_class substrate_equivalence_class_fusion(
        struct substrate_equivalence_class ec1,
        struct substrate_equivalence_class ec2)
{
    struct substrate_equivalence_class res;

    res.size = ec1.size + ec2.size;
    res.array_references = NULL;
    osl_relation_list_add(
            &res.array_references,
            osl_relation_list_clone(ec1.array_references));
    osl_relation_list_add(
            &res.array_references,
            osl_relation_list_clone(ec2.array_references));
    
    return res;
}


/**
 * @brief Count the total number of reference/access relation in an
 * substrate_uniformly_generated_set.
 *
 * @param[in] ugs
 *
 * @return 
 */
unsigned int substrate_uniformly_generated_set_count_access(
        struct substrate_uniformly_generated_set ugs)
{
    unsigned int res = 0;
    unsigned int i = 0;

    for(i=0 ; i<ugs.size ; i++)
    {
        res += ugs.classes[i].size;
    }

    return res;
}


/**
 * @brief Count recursively the total number of reference/access relation in
 * a substrate_array_profile.
 *
 * @param[in] ap
 *
 * @return 
 */
unsigned int substrate_array_profile_count_access(
        struct substrate_array_profile ap)
{
    unsigned int res = 0;
    unsigned int i = 0;

    for(i=0 ; i<ap.size ; i++)
    {
        res += substrate_uniformly_generated_set_count_access(ap.uniformly_gen_sets[i]);
    }

    return res;
}
