#ifndef __SUBSTRATE__OPTIONS_H
#define __SUBSTRATE__OPTIONS_H

#include <stdio.h>
#include <stdbool.h>

/**
 * A structure containing all the options informations and structure.
 */
struct substrate_options
{
    FILE * input_file;          /**<The input file used to construct the osl_scop */
    FILE * output_file;         /**<The output file/stream where will be written the result */
    bool row_major;             /**<A boolean, true when the array references are analyzed as row-major, false if column-major */
    double minimal_reuse_rate;  /**<The minimal reuse rate necessary to aggregate 2 statements*/
};

/**
 * An global instance of a substrate_options to be accessed everywhere we need to get
 * the options.
 */
extern struct substrate_options g_substrate_options;


void substrate_options_init(
        int argc,
        char** argv);
void substrate_print_help(
        char *program_name,
        FILE *output);
void substrate_option_terminate();


#endif
