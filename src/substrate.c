#include <stdio.h>
#include <stdlib.h>

#include <osl/scop.h>
#include <osl/macros.h>
#include <osl/util.h>
#include <osl/extensions/dependence.h>

#include "substrate.h"
#include "options.h"
#include "statement_profile.h"
#include "analyze.h"
#include "optimization.h"

int main(int argc,char** argv)
{
    osl_scop_p input_scop = NULL;
    osl_scop_p output_scop = NULL;
    struct substrate_statement_profile * statement_profiles = NULL;

    substrate_parse_options(argc,argv);


    input_scop = osl_scop_read(g_substrate_options.input_file);
    statement_profiles = substrate_analyze(input_scop);
    output_scop = substrate_optimize(statement_profiles,input_scop);
    osl_scop_print(stdout,output_scop);


    osl_scop_free(input_scop);
    osl_scop_free(output_scop);

    return 0;
}
