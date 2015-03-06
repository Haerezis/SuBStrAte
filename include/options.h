#ifndef __OPTIONS_H
#define __OPTIONS_H

#include <stdio.h>
#include <stdbool.h>

struct substrate__options
{
    FILE * input_file;
    FILE * output_file;
    bool row_major;
};

extern struct substrate__options substrate_options;


void substrate__parse_options(int argc, char** argv);
void substrate__print_help(char *program_name, FILE *output);


#endif
