#include <stdlib.h>
#include <string.h>

#include "substrate/statement_profile.h"
#include "substrate/parallelism.h"
#include "substrate/vectorization.h"
#include "substrate/utils.h"


/**
 * @brief Allocate a new and blank substrate_statement_profile.
 *
 * @return The freshly allocated substrate_statement_profile.
 */
struct substrate_statement_profile * substrate_statement_profile_malloc()
{
    struct substrate_statement_profile * res = NULL;

    res = (struct substrate_statement_profile*)
        malloc(sizeof(struct substrate_statement_profile));

    memset(res, 0, sizeof(struct substrate_statement_profile));
    
    return res;
}


/**
 * @brief Clone (hard copy) an osl_scop, but also clone the substrate_statement_profile
 * pointed by every osl_statement->usr field.
 *
 * @param[in] scop The osl_scop that will be cloned.
 *
 * @return A clone of \a scop including the substrate_statement_profile pointed by the
 * osl_statement->usr filed.
 */
struct osl_scop * substrate_osl_scop_clone(struct osl_scop * scop)
{
    struct osl_scop * res = NULL, *inital_res = NULL;
    struct osl_statement *stmt1 = NULL, *stmt2 = NULL;

    res = osl_scop_clone(scop);
    //inital_res point to the first scop of the cloned list, and that's what will be returned
    //at the end of the function.
    inital_res = res;

    while(res != NULL)
    {
        stmt1 = scop->statement;
        stmt2 = res->statement;
        while((stmt1 != NULL) && (stmt2 != NULL))
        {
            stmt2->usr = substrate_statement_profile_clone(stmt1->usr);
            stmt1 = stmt1->next;
            stmt2 = stmt2->next;
        }
        res = res->next;
    }
    
    return inital_res;
}


/**
 * @brief Make a hard and recursive (clone all the sub-profile) copy of a
 * substrate_statement_profile.
 *
 * @param[in] stmt_profile The statement profile that will be cloned.
 *
 * @return The clone of stmt_profile.
 */
struct substrate_statement_profile * substrate_statement_profile_clone(
        struct substrate_statement_profile * stmt_profile)
{
    struct substrate_statement_profile * res = NULL;

    res = substrate_statement_profile_malloc();

    res->reuse = substrate_reuse_profile_clone(stmt_profile->reuse);
    res->parallelism = substrate_parallelism_profile_clone(&stmt_profile->parallelism);
    res->vectorization = substrate_vectorization_profile_clone(&stmt_profile->vectorization);

    return res;
}


/**
 * @brief Allocate and construct a statement profile from an osl_statement
 * by analyzing it.
 * This function allocated and call the constructor function of all the
 * sub-profile (reuse, parallelism,...).
 *
 * @param scop The scop of the statement that will be analyzed.
 * @param stmt The statement analyzed to create the substrate_statement_profile.
 *
 * @return A statement_profile containing all the informations gathered during
 * the analyze.
 */
struct substrate_statement_profile * substrate_statement_profile_constructor(
        struct osl_scop * scop,
        struct osl_statement * stmt)
{
    struct substrate_statement_profile * res = NULL;

    res = substrate_statement_profile_malloc();

    ////////////////////
    // Different profiling functions go here

    res->reuse = substrate_reuse_profile_constructor(stmt);
    res->parallelism = substrate_parallelism_profile_constructor(scop, stmt);
    res->vectorization = substrate_vectorization_profile_constructor(scop, stmt);
    
    // End of profiling
    ///////////////////
    return res;
}




/**
 * @brief Aggregate two statements into a third one 
 * (aggregating the osl_statement and the profiles).
 *
 * @param[in] stmt1 The first statement that will be aggregated.
 * @param[in] stmt2 The second statement that will be aggregated.
 *
 * @return A new osl_statement, result of the aggregation of the arguments (even the profiles).
 */
struct osl_statement * substrate_statement_fusion(
        struct osl_statement * stmt1,
        struct osl_statement * stmt2)
{
    struct osl_statement * res = NULL;
    struct substrate_statement_profile *profile1 = NULL, *profile2 = NULL;

    res = substrate_osl_statement_fusion(stmt1, stmt2);

    profile1 = (struct substrate_statement_profile *)stmt1->usr;
    profile2 = (struct substrate_statement_profile *)stmt2->usr;
    res->usr = substrate_statement_profile_fusion(profile1 , profile2);
    
    return res;
}


/**
 * @brief Aggregate two statements profile into one.
 *
 * @param[in] stmt1 The first statement profile of the aggregation.
 * @param[in] stmt2 The second statement profile of the aggregation.
 *
 * @return A statement profile created from the aggregation of \a stmt1 and \a stmt2 .
 */
struct substrate_statement_profile * substrate_statement_profile_fusion(
        struct substrate_statement_profile * stmt1,
        struct substrate_statement_profile * stmt2)
{
    struct substrate_statement_profile * res;

    res = substrate_statement_profile_malloc();
    res->reuse = substrate_reuse_profile_fusion(
            stmt1->reuse,
            stmt2->reuse);
    res->parallelism = substrate_parallelism_profile_fusion(
            &stmt1->parallelism,
            &stmt2->parallelism);
    res->vectorization = substrate_vectorization_profile_fusion(
            &stmt1->vectorization,
            &stmt2->vectorization);
    
    return res;
}


/**
 * @brief Aggregate two osl_statement into one.
 *
 * A new osl_statement is created from the two osl_statement given as parameters.
 * The parameters are not modified.
 *
 * @param[in] stmt1 The first osl_statement of the aggregation.
 * @param[in] stmt2 The second osl_statement of the aggregation.
 *
 * @return A new osl_statement created from the aggregation of \a stmt1 and \a stmt2 .
 */
struct osl_statement * substrate_osl_statement_fusion(
        struct osl_statement * stmt1,
        struct osl_statement * stmt2)
{
    struct osl_statement * res = NULL;
    //For now, the two statement MUST have the same domain & scattering
    //TODO : allow aggregation of statement with diff domain (→replace the clone
    //of the domain [and scattering]).
    res = osl_statement_malloc();
    res->domain = osl_relation_clone(stmt1->domain);
    res->scattering = osl_relation_clone(stmt1->scattering);
    res->access = NULL;
    osl_relation_list_add(
            &res->access,
            osl_relation_list_clone(stmt1->access));
    osl_relation_list_add(
            &res->access,
            osl_relation_list_clone(stmt2->access));
    res->extension = substrate_osl_generic_fusion(stmt1->extension, stmt2->extension);
    res->usr = NULL;
    res->next = stmt2->next;
    
    return res;
}


/**
 * @brief Aggregate two osl_generic together (cloning the necessary structures).
 * The parameters are not modified. If two generics are found, only one is kept (like a union).
 *
 * @param gen1 A pointer to the first osl_generic.
 * @param gen2 A pointer to the first osl_generic.
 *
 * @return The aggregation of two osl_generic.
 */
struct osl_generic * substrate_osl_generic_fusion(
        struct osl_generic * gen1,
        struct osl_generic * gen2)
{
    struct osl_generic * res = NULL, *tmp1 = NULL, *tmp2 = NULL;
    struct osl_generic * new_generic = NULL;

    tmp1 = gen1;
    while(tmp1 != NULL)
    {
        tmp2 = substrate_osl_generic_lookup(gen2, tmp1->interface->URI);
        if(tmp2 == NULL)
        {
            osl_generic_add(&res, substrate_osl_generic_nclone(tmp1,1));
        }
        else if(osl_generic_equal(tmp1, tmp2))
        {
            if(strcmp(tmp1->interface->URI,"body") == 0)
            {
                new_generic = osl_generic_malloc();
                new_generic->interface = osl_body_interface();
                new_generic->next = NULL;
                new_generic->data = substrate_osl_body_fusion(tmp1->data,tmp2->data);

                osl_generic_add(&res, new_generic);
            }
            else
            {
                osl_generic_add(&res, substrate_osl_generic_nclone(tmp1,1));
            }
        }
        else
        {
            if(strcmp(tmp1->interface->URI,"body") == 0)
            {
                new_generic = osl_generic_malloc();
                new_generic->interface = osl_body_interface();
                new_generic->next = NULL;
                new_generic->data = substrate_osl_body_fusion(tmp1->data,tmp2->data);

                osl_generic_add(&res, new_generic);
            }
            else
            {
                OSL_warning("Can't fusion the generic structures : don't know how.");
                fprintf(stderr,"URI : %s\n", tmp1->interface->URI);
            }
        }
        tmp1 = tmp1->next;
    }

    return res;
}


/**
 * @brief Fusion two osl_body into a third one.
 * The two osl_body must have the same iterators or the program will fail.
 *
 * @param body1 The first osl_body.
 * @param body2 The second osl_body.
 *
 * @return An osl_body with the same iteratar as the parameters, but with
 * their "code" fusionned.
 */
struct osl_body * substrate_osl_body_fusion(
        struct osl_body * body1,
        struct osl_body * body2)
{
    struct osl_body * res = NULL;
    size_t body1_size = 0, body2_size = 0;

    body1_size = osl_strings_size(body1->iterators);
    body2_size = osl_strings_size(body2->iterators);

    res = osl_body_malloc();
    if((osl_strings_equal(body1->iterators, body2->iterators)) || (body1_size==0) || (body2_size==0))
    {
        res->iterators = osl_strings_clone(body1->iterators);
        substrate_osl_strings_concat(
                &res->expression,
                body1->expression,
                body2->expression);
    }
    else
    {
        //TODO Maybe fuse when iterators don't match ? (I'm not really sure).
        OSL_error("Can't fusion body");
    }

    return res;
}

/**
 * @brief Free recursively the scop profiles : meaning all the statement profiles.
 *
 * @param[in] sp The osl_scop containing the statement that will have their profile freed.
 */
void substrate_scop_profile_free(
        struct osl_scop * sp)
{
    struct osl_statement * stmt = NULL;

    stmt = sp->statement;
    while(stmt != NULL)
    {
        substrate_statement_profile_free((struct substrate_statement_profile*)stmt->usr);
        stmt->usr = NULL;
        stmt = stmt->next;
    }
}


/**
 * @brief Free recursively an substrate_statement_profile.
 *
 * @param sp The substrate_statement_profile that is freed.
 */
void substrate_statement_profile_free(
        struct substrate_statement_profile * sp)
{
    substrate_reuse_profile_free(&sp->reuse);
    substrate_parallelism_profile_free(&sp->parallelism);
    substrate_vectorization_profile_free(&sp->vectorization);
    free(sp);
}
