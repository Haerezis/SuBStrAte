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
    struct substrate_scop_profile scop_profile;

    substrate_options_init(argc, argv);


    input_scop = osl_scop_read(g_substrate_options.input_file);
    scop_profile = substrate_analyze(input_scop);
    output_scop = substrate_optimize(scop_profile);
    osl_scop_print(g_substrate_options.output_file, output_scop);

    osl_scop_free(input_scop);
    osl_scop_free(output_scop);
//    substrate_scop_profile_free(&scop_profile);//FIXME
    substrate_option_terminate();

    return 0;
}
