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
struct osl_scop * substrate_analyze(struct osl_scop * scop)
{
    struct osl_statement *statement = NULL;
    struct osl_scop * profiled_scop = NULL;

    profiled_scop = osl_scop_clone(scop);

    //Analyzing the reuse,parallel.. informations of every statement of the scop
    //and put them in their corresponding statement_profile
    statement = profiled_scop->statement;
    while(statement != NULL)
    {
        statement->usr = substrate_statement_profile_constructor(statement);

        statement = statement->next;
    }
        
    //Dump internal structures to stdout
    /*for(i=0; i<profiled_scop.size ; i++)*/
    /*{*/
        /*fprintf(stdout,"\n================= DUMP S%d ==================\n",i+1);*/
/*//        osl_statement_dump(stdout,statement_profiles[i].osl_statement);*/
        /*substrate_reuse_profile_dump(stdout,&statement_profiles[i].reuse);*/
        /*fprintf(stdout,"============================================\n");*/
    /*}*/

    return profiled_scop;
}
