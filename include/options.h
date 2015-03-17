#ifndef __SUBSTRATE__OPTIONS_H
#define __SUBSTRATE__OPTIONS_H

#include <stdio.h>
#include <stdbool.h>

struct substrate_options
{
    FILE * input_file;
    FILE * output_file;
    bool row_major;
    double minimal_reuse_rate;
};

extern struct substrate_options g_substrate_options;


void substrate_parse_options(
        int argc,
        char** argv);

void substrate_print_help(
        char *program_name,
        FILE *output);


void substrate_option_free();


#endif
