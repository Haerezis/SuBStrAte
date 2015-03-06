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
    substrate__parse_options(argc,argv);

    printf("Hello World !\n");
    return 0;
}
