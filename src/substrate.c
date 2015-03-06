#include <stdio.h>
#include <stdlib.h>

#include <osl/scop.h>
#include <osl/macros.h>
#include <osl/util.h>
#include <osl/extensions/dependence.h>

#include "substrate.h"
#include "options.h"

int main(int argc,char** argv)
{
    osl_scop_p input_scop = NULL;
    osl_scop_p output_scop = NULL;

    substrate__parse_options(argc,argv);


    input_scop = osl_scop_read(g_substrate__options.input_file);
    output_scop = osl_scop_clone(input_scop);
    osl_scop_print(stdout,output_scop);


    osl_scop_free(input_scop);
    osl_scop_free(output_scop);

    return 0;
}
