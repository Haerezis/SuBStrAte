#include <stdlib.h>

#include "statement_profile.h"
#include "utils.h"


/**
 * @brief Aggregate two statement' profile into one.
 * The osl_statements carry contained in the profiles are also aggregated.
 *
 * @param[in] stmt1 The first statement profile of the aggregation.
 * @param[in] stmt2 The second statement profile of the aggregation.
 *
 * @return A statement profile created from the aggregation of \a stmt1 and \a stmt2 .
 */
struct substrate_statement_profile substrate_statement_profile_fusion(
        struct substrate_statement_profile stmt1,
        struct substrate_statement_profile stmt2)
{
    struct substrate_statement_profile res;

    res.osl_statement = substrate_osl_statement_fusion(
            stmt1.osl_statement,
            stmt2.osl_statement);
    
    res.reuse = substrate_reuse_profile_fusion(stmt1.reuse, stmt2.reuse);
    
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
            osl_relation_list_clone(stmt1->access));
    res->extension = NULL;
    osl_generic_add(
            &res->extension,
            stmt1->extension);
    osl_generic_add(
            &res->extension,
            stmt2->extension);
    res->usr = NULL;
    res->next = stmt2->next;
    
    return res;
}



/**
 * @brief Free recursively an substrate_scop_profile.
 *
 * @param[in] sp The substrate_scop_profile that is freed.
 */
void substrate_scop_profile_free(
        struct substrate_scop_profile * sp)
{
    unsigned int i = 0;

    for(i=0; i<sp->size ; i++)
    {
        substrate_statement_profile_free(&sp->statement_profiles[i]);
    }
    free(sp->statement_profiles);
    sp->statement_profiles = NULL;
    sp->size = 0;
}


/**
 * @brief Free recursively an substrate_statement_profile.
 *
 * @param sp The substrate_statement_profile that is freed.
 */
void substrate_statement_profile_free(
        struct substrate_statement_profile * sp)
{
    osl_statement_free(sp->osl_statement);
    sp->osl_statement = NULL;
    substrate_reuse_profile_free(&sp->reuse);
}


/**
 * @brief Convert a substrate_scop_profile into a osl_scop.
 *
 * @param[in] scop_profile The substrate_scop_profile that is converted.
 *
 * @return The equivalent of \a scop_profile as a osl_scop structure.
 */
struct osl_scop * substrate_scop_profile_to_osl_scop(struct substrate_scop_profile scop_profile)
{
    unsigned int i = 0;
    struct osl_scop * res = NULL;

    res = osl_scop_malloc();
    substrate_copy_scop_except_statements(res, scop_profile.scop);

    res->statement = NULL;
    for(i=0 ; i<scop_profile.size ; i++)
    {
        osl_statement_add(
                &res->statement,
                osl_statement_clone(scop_profile.statement_profiles[i].osl_statement));
    }

    return res;
}
