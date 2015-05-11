#include "substrate/tiling_hyperplane.h"

#include "substrate/utils.h"
#include "pluto.h"
#include "program.h"
#include <stdbool.h>


struct substrate_tiling_hyperplane_profile substrate_tiling_hyperplane_profile_constructor(
       struct osl_scop * scop,
       struct osl_statement * statement)
{
    struct substrate_tiling_hyperplane_profile res = {NULL, 0, 0};
    PlutoProg *prog = NULL;
    struct osl_scop * pluto_scop = NULL;
    
    //Creating a dummy scop with only the current analyzed statement for pluto.
    //Also reset the beta depth of the statement, because it's supposed to be
    //alone now.
    pluto_scop = substrate_osl_scop_nclone_expect_statement(scop, 1);
    pluto_scop->statement = osl_statement_nclone(statement,1);
    substrate_reset_beta_depth(pluto_scop->statement);

    //options is a pluto's global variable (go see main.c from pluto project).
    options = pluto_options_alloc();
    options->tile = 1;
    prog = scop_to_pluto_prog(pluto_scop, options);
    pluto_auto_transform(prog);
    pluto_detect_transformation_properties(prog);

    res = substrate_tiling_hyperplane_profile_clone_PlutoMatrix(prog->stmts[0]->trans);

    pluto_prog_free(prog);
    pluto_options_free(options);

    osl_scop_free(pluto_scop);

    return res;
}

double substrate_rate_tiling_hyperplane_profiles(
        struct substrate_tiling_hyperplane_profile thp1,
        struct substrate_tiling_hyperplane_profile thp2)
{
    double res = 0.0;
    bool total_match = false;

    if((thp1.nb_rows == thp2.nb_rows) && (thp1.nb_columns == thp2.nb_columns))
    {
        total_match = true;
        for(unsigned int i = 0; (i<thp1.nb_rows) && total_match ; i++)
        {
            for(unsigned int j = 0 ; (j<thp2.nb_rows) && total_match ; j++)
            {
                total_match = thp1.vectors[i][j] == thp2.vectors[i][j];
            }
        }
        res = (double)total_match;
    }
    
    return res;
}
