#include <stdlib.h>

#include "analyze.h"


/**
 * @brief Analyze \a scop and generate a profile for every statement inside it. These
 *         profiles will be used during the optimization pass later.
 *
 * @param[in] scop The scop that will be analyzed.
 *
 * @return The scop profile, containing a profile for every statement of the scop.
 */
struct substrate_scop_profile substrate_analyze(struct osl_scop * scop)
{
    unsigned int i = 0;
    struct osl_statement *statement = NULL;

    struct substrate_scop_profile scop_profile;
    struct substrate_statement_profile * statement_profiles = NULL;

    //Counting the number of statement in the scop for the allocation
    //of the substrate_statement_profile array.
    statement = scop->statement;
    for(i=0; statement != NULL ; statement=statement->next, i++);
    scop_profile.size = i;
    scop_profile.statement_profiles = (struct substrate_statement_profile*)
        malloc(scop_profile.size * sizeof(struct substrate_statement_profile));
    scop_profile.scop = scop;



    //Analyzing the reuse,parallel.. informations of every statement of the scop
    //and put them in their corresponding statement_profile
    statement_profiles = scop_profile.statement_profiles;
    statement = scop->statement;
    for(i=0; i<scop_profile.size ; i++)
    {
        statement_profiles[i].osl_statement = osl_statement_nclone(statement,1);
        statement_profiles[i].reuse = substrate_reuse_profile_constructor(statement);

        statement = statement->next;
    }
        
    //Dump internal structures to stdout
    /*for(i=0; i<scop_profile.size ; i++)*/
    /*{*/
        /*fprintf(stdout,"\n================= DUMP S%d ==================\n",i+1);*/
/*//        osl_statement_dump(stdout,statement_profiles[i].osl_statement);*/
        /*substrate_reuse_profile_dump(stdout,&statement_profiles[i].reuse);*/
        /*fprintf(stdout,"============================================\n");*/
    /*}*/

    return scop_profile;
}
