#include <stdlib.h>

#include "substrate/analyze.h"


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
    struct osl_scop * profiled_scop = NULL, *initial_profiled_scop = NULL;
    struct substrate_statement_profile * stmt_prof = NULL;

    profiled_scop = osl_scop_clone(scop);
    initial_profiled_scop = profiled_scop;

    //For every scop in the scop list
    while(profiled_scop != NULL)
    {
        //Analyzing the reuse,parallel.. informations of every statement of the scop
        //and put them in their corresponding statement_profile
        statement = profiled_scop->statement;
        while(statement != NULL)
        {
            stmt_prof = substrate_statement_profile_constructor(scop, statement);
            statement->usr = stmt_prof;

            statement = statement->next;
        }
            
        //Dump internal structures to stdout
        statement = profiled_scop->statement;
        unsigned int i = 1;
        while(statement != NULL)
        {
            stmt_prof = (struct substrate_statement_profile*)statement->usr;
            fprintf(stdout,"\n================= DUMP S%d ==================\n",i);
            fprintf(stdout,"body : ");
            osl_generic_print(stdout,statement->extension);
            /*substrate_reuse_profile_dump(stdout,&statement_profiles[i].reuse);*/
            substrate_parallelism_profile_dump(stdout,&stmt_prof->parallelism);
            substrate_vectorization_profile_dump(stdout,&stmt_prof->vectorization);
            fprintf(stdout,"============================================\n");

            statement = statement->next;
            i++;
        }
        
        profiled_scop = profiled_scop->next;
    }

    return initial_profiled_scop;
}
