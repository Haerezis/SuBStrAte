#include "substrate/options.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "substrate/enum.h"

# ifdef __cplusplus
extern "C"
{
#endif

struct substrate_options g_substrate_options;


/**
 * @brief Create a substrate_options structure with default options set.
 *
 * @return A default set options structure.
 */
struct substrate_options substrate_options_default()
{
    struct substrate_options options;
    memset(&options, 0, sizeof(options));

    //base options
    options.input_file = NULL;
    options.output_file = stdout;
    options.row_major = true;
    options.dump = false;

    options.minimal_reuse_rate = 0.0;
    options.minimal_parallelism_rate = 0.0;
    options.minimal_vectorization_rate = 0.0;
    options.minimal_tiling_hyperplane_rate = 0.0;
    
    options.minimal_rate = 1.0;
    options.reuse_weight = 0.0;
    options.parallelism_weight = 0.0;
    options.vectorization_weight = 0.0;
    options.tiling_hyperplane_weight = 0.0;

    options.aggregation_strategy = SIMPLE_SUCCESSIVE_AGGREGATION;

    return options;
}

/**
 * @brief Parse the program argument to initialize the options of the program.
 *
 * @param[in] argc The number of options/arguments of the program.
 * @param[in] argv The options/arguments of the program.
 */
void substrate_options_init(
        int argc,
        char** argv)
{
    int i = 0;
    bool arguments_check = 1;

    g_substrate_options = substrate_options_default();

    for(i=1 ; i<argc ; i++)
    {
        if( (strcmp(argv[i],"-h") && strcmp(argv[i],"--help")) == 0 )
        {
            substrate_print_help(argv[0],stdout);
            exit(EXIT_SUCCESS);
        }
        else if( (strcmp(argv[i],"-o") && strcmp(argv[i],"--output")) == 0 )
        {
            if(((i+1) < argc) && (argv[i+1][0] != '-'))
            {
                g_substrate_options.output_file = fopen(argv[i+1],"w");
                i++;
            }
            else
            {
                fprintf(stderr,"No output filename has been given\n");
                substrate_print_help(argv[0],stderr);
                exit(EXIT_FAILURE);
            }
        }
        else if(strcmp(argv[i],"--column-major") == 0)
        {
            g_substrate_options.row_major = 0;
        }
        /*else if( (strcmp(argv[i],"-mrr") && strcmp(argv[i],"--min-reuse-rate")) == 0 )*/
        /*{*/
            /*if(((i+1) < argc) && (argv[i+1][0] != '-'))*/
            /*{*/
                /*g_substrate_options.minimal_reuse_rate = strtod(argv[i+1],NULL);*/
                /*i++;*/
            /*}*/
            /*else*/
            /*{*/
                /*fprintf(stderr,"No minimal reuse rate has been given\n");*/
                /*substrate_print_help(argv[0],stderr);*/
                /*exit(EXIT_FAILURE);*/
            /*}*/
        /*}*/
        else if( (strcmp(argv[i],"-mr") && strcmp(argv[i],"--min-rate")) == 0 )
        {
            if(((i+1) < argc) && (argv[i+1][0] != '-'))
            {
                g_substrate_options.minimal_rate = strtod(argv[i+1],NULL);
                i++;
            }
            else
            {
                fprintf(stderr,"No minimal rate has been given "
                        "even though the parameter has been set\n");
                substrate_print_help(argv[0],stderr);
                exit(EXIT_FAILURE);
            }
        }
        else if( (strcmp(argv[i],"-rw") && strcmp(argv[i],"--reuse-weight")) == 0 )
        {
            if(((i+1) < argc) && (argv[i+1][0] != '-'))
            {
                g_substrate_options.reuse_weight = strtod(argv[i+1],NULL);
                i++;
            }
            else
            {
                fprintf(stderr,"No reuse weight has been given"
                        " even though parameter has been set\n");
                substrate_print_help(argv[0],stderr);
                exit(EXIT_FAILURE);
            }
        }
        else if( (strcmp(argv[i],"-pw") && strcmp(argv[i],"--paral-weight")) == 0 )
        {
            if(((i+1) < argc) && (argv[i+1][0] != '-'))
            {
                g_substrate_options.parallelism_weight = strtod(argv[i+1],NULL);
                i++;
            }
            else
            {
                fprintf(stderr,"No parallelism weight has been given"
                        " even though parameter has been set\n");
                substrate_print_help(argv[0],stderr);
                exit(EXIT_FAILURE);
            }
        }
        else if( (strcmp(argv[i],"-vw") && strcmp(argv[i],"--vector-weight")) == 0 )
        {
            if(((i+1) < argc) && (argv[i+1][0] != '-'))
            {
                g_substrate_options.vectorization_weight = strtod(argv[i+1],NULL);
                i++;
            }
            else
            {
                fprintf(stderr,"No vectorization weight has been given"
                        " even though parameter has been set\n");
                substrate_print_help(argv[0],stderr);
                exit(EXIT_FAILURE);
            }
        }
        else if( (strcmp(argv[i],"-tw") && strcmp(argv[i],"--tiling-weight")) == 0 )
        {
            if(((i+1) < argc) && (argv[i+1][0] != '-'))
            {
                g_substrate_options.tiling_hyperplane_weight = strtod(argv[i+1],NULL);
                i++;
            }
            else
            {
                fprintf(stderr,"No tiling hyperplane weight has been given"
                        " even though parameter has been set\n");
                substrate_print_help(argv[0],stderr);
                exit(EXIT_FAILURE);
            }
        }
        else if( (strcmp(argv[i],"-ssa") && strcmp(argv[i],"--simple-successive-aggregation")) == 0)
        {
            g_substrate_options.aggregation_strategy = SIMPLE_SUCCESSIVE_AGGREGATION;
        }
        else if( (strcmp(argv[i],"-gga") && strcmp(argv[i],"--greedy-graph-aggregation")) == 0)
        {
            g_substrate_options.aggregation_strategy = GREEDY_GRAPH_AGGREGATION;
        }
        else if(strcmp(argv[i],"--dump") == 0)
        {
            g_substrate_options.dump = true;
        }
        //else if(strcmp(argv[i],"") == 0)
        else if(argv[i][0] == '-')
        {
            fprintf(stderr,"Unknown options '%s'\n",argv[i]);
            substrate_print_help(argv[0],stderr);
            exit(EXIT_FAILURE);
        }
        else if(g_substrate_options.input_file == NULL)
        {
            g_substrate_options.input_file = fopen(argv[i],"r");
        }
    }

    //Test if required arguments were provided
    arguments_check = arguments_check && g_substrate_options.input_file != NULL;

    if(arguments_check != 1)
    {
        fprintf(stderr,"Some required arguments are missing\n");
        substrate_print_help(argv[0],stderr);
        exit(EXIT_FAILURE);
    }
}


/**
 * @brief Print the help and usage of the program.
 *
 * @param[in] program_name The name of the program of which the help/usage will be printed.
 * @param[in] output The stream to output the help/usage.
 */
void substrate_print_help(
        char *program_name,
        FILE * output)
{
    fprintf(output,"Usage : %s", program_name);
    fprintf(output," [-h]");
    fprintf(output," [-o FILE]");
    fprintf(output," [--column-major]");
    fprintf(output," [--dump]");
    fprintf(output," INPUT_FILE");
    fprintf(output,"\n\n");

    fprintf(output,"--dump\n");
    fprintf(output,"\tDump the internal structure of the different profiles\n");

    fprintf(output,"-ssa, --simple-successive-aggregation\n");
    fprintf(output,"\tUse the Simple Successive Aggregation algorithm during aggregation optimization\n");

    fprintf(output,"-gga, --greedy-graph-aggregation\n");
    fprintf(output,"\tUse the Greedy Graph Aggregation algorithm during aggregation optimization\n");

    fprintf(output,"-h, --help\n");
    fprintf(output,"\tPrint a usage message briefly summarizing these command-line options then exit.\n");
    
    fprintf(output,"-o FILE, --output FILE\n");
    fprintf(output,"\tPlace output in file FILE.\n");
    fprintf(output,"\tIf this options is not specified, stdout will be used\n");

    /*fprintf(output,"-mrr N, --min-reuse-rate N\n");*/
    /*fprintf(output,"\tThe reuse rate needed for 2 statements tu be aggregated.\n");*/
    
    fprintf(output,"-mr N, --min-rate N\n");
    fprintf(output,"\tN is a float, 0.0 <= N <= 1.0\n");
    fprintf(output,"\tThe global rate (weighted average of all the rates) needed for 2 statements tu be aggregated.\n");

    fprintf(output,"-rw N, --reuse-weight N\n");
    fprintf(output,"\tN is a float, 0 <= N\n");
    fprintf(output,"\tThe weight of the reuse rate used in the weighted average rate.\n");
    
    fprintf(output,"-pw N, --paral-weight N\n");
    fprintf(output,"\tN is a float, 0 <= N\n");
    fprintf(output,"\tThe weight of the parallelism rate used in the weighted average rate.\n");
    
    fprintf(output,"-vw N, --vector-weight N\n");
    fprintf(output,"\tN is a float, 0 <= N\n");
    fprintf(output,"\tThe weight of the vectorization rate used in the weighted average rate.\n");
    
    fprintf(output,"-tw N, --tiling-weight N\n");
    fprintf(output,"\tN is a float, 0 <= N\n");
    fprintf(output,"\tThe weight of the tiling hyperplane rate used in the weighted average rate.\n");
    
    fprintf(output,"--column-major\n");
    fprintf(output,"\tUse the column-major layout for the analyse and optimization of the different arrays\n");
    fprintf(output,"\tIf this option is not specified, row-major layout is used as default\n");
    
    fprintf(output,"INPUT_FILE\n");
    fprintf(output,"\tThe OpenScop file to analyse and optimize\n");
}


/**
 * @brief Terminate and free the options of the program.
 */
void substrate_option_terminate()
{
    fclose(g_substrate_options.input_file);
    fclose(g_substrate_options.output_file);
}


#ifdef __cplusplus
}
#endif
