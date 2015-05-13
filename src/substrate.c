#include <stdio.h>
#include <stdlib.h>

#include <osl/scop.h>
#include <osl/macros.h>
#include <osl/util.h>
#include <osl/extensions/dependence.h>

#include "candl/candl.h"

#include "substrate/substrate.h"
#include "substrate/options.h"
#include "substrate/statement_profile.h"
#include "substrate/analyze.h"
#include "substrate/optimization.h"

int main(int argc,char** argv)
{
    osl_scop_p input_scop = NULL;
    osl_scop_p profiled_scop = NULL;
    osl_scop_p output_scop = NULL;

    substrate_options_init(argc, argv);


    input_scop = osl_scop_read(g_substrate_options.input_file);
    profiled_scop = substrate_analyze(input_scop);
    output_scop = substrate_optimize(profiled_scop);
    osl_scop_print(g_substrate_options.output_file, output_scop);

    osl_scop_free(input_scop);
    substrate_scop_profile_free(output_scop);
    osl_scop_free(output_scop);
    substrate_scop_profile_free(profiled_scop);
    osl_scop_free(profiled_scop);
    substrate_option_terminate();

    pip_close();//Necessary because candl doesn't do it automatically.

    return 0;
}
