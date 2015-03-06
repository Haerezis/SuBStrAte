#include "options.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

# ifdef __cplusplus
extern "C"
{
#endif

struct substrate__options g_substrate__options;


void substrate__parse_options(int argc, char** argv)
{
    int i = 0;
    bool arguments_check = 1;

    //base options
    g_substrate__options.input_file = NULL;
    g_substrate__options.output_file = stdout;
    g_substrate__options.row_major = 1;

    for(i=1 ; i<argc ; i++)
    {
        if( (strcmp(argv[i],"-h") && strcmp(argv[i],"--help")) == 0 )
        {
            substrate__print_help(argv[0],stdout);
            exit(EXIT_SUCCESS);
        }
        else if( (strcmp(argv[i],"-o") && strcmp(argv[i],"--output")) == 0 )
        {
            if(((i+1) < argc) && (argv[i+1][0] != '-'))
            {
                g_substrate__options.output_file = fopen(argv[i+1],"w");
                i++;
            }
            else
            {
                fprintf(stderr,"No output filename has beed given\n");
                substrate__print_help(argv[0],stderr);
                exit(EXIT_FAILURE);
            }
        }
        else if(strcmp(argv[i],"--column-major") == 0)
        {
            g_substrate__options.row_major = 0;
        }
        //else if(strcmp(argv[i],"") == 0)
        else if(argv[i][0] == '-')
        {
            fprintf(stderr,"Unknown options '%s'\n",argv[i]);
            substrate__print_help(argv[0],stderr);
            exit(EXIT_FAILURE);
        }
        else if(g_substrate__options.input_file == NULL)
        {
            g_substrate__options.input_file = fopen(argv[i],"r");
        }
    }

    //Test if required arguments were provided
    arguments_check = arguments_check && g_substrate__options.input_file != NULL;

    if(arguments_check != 1)
    {
        fprintf(stderr,"Some required arguments are missing\n");
        substrate__print_help(argv[0],stderr);
        exit(EXIT_FAILURE);
    }
}

void substrate__print_help(char *program_name,FILE * output)
{
    fprintf(output,"Usage : %s", program_name);
    fprintf(output," [-h]");
    fprintf(output," [-o FILE]");
    fprintf(output," [--column-major]");
    fprintf(output," INPUT_FILE");
    fprintf(output,"\n\n");

    fprintf(output,"-h, --help\n");
    fprintf(output,"\tPrint a usage message briefly summarizing these command-line options then exit.\n");
    fprintf(output,"-o FILE, --output FILE\n");
    fprintf(output,"\tPlace output in file FILE.\n");
    fprintf(output,"\tIf this options is not specified, stdout will be used\n");
    fprintf(output,"--column-major\n");
    fprintf(output,"\tUse the column-major layout for the analyse and optimization of the different arrays\n");
    fprintf(output,"\tIf this option is not specified, row-major layout is used as default\n");
    fprintf(output,"INPUT_FILE\n");
    fprintf(output,"\tThe OpenScop file to analyse and optimize\n");
}


#ifdef __cplusplus
}
#endif
