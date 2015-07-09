#ifndef __SUBSTRATE__OPTIONS_H
#define __SUBSTRATE__OPTIONS_H

#include <stdio.h>
#include <stdbool.h>
#include "substrate/enum.h"

/**
 * A structure containing all the options informations and structure.
 */
struct substrate_options
{
    FILE * input_file;          /**<The input file used to construct the osl_scop */
    FILE * output_file;         /**<The output file/stream where will be written the result */
    bool row_major;             /**<A boolean, true when the array references are analyzed as row-major, false if column-major */
    bool dump;                  /*<A boolean, true when after the analyze the program dump the internal infos */

    double minimal_reuse_rate;  /**<The minimal reuse rate necessary to aggregate 2 statements*/
    double minimal_parallelism_rate;  /**<The minimal parallelism rate necessary to aggregate 2 statements*/
    double minimal_vectorization_rate;  /**<The minimal vectorization rate necessary to aggregate 2 statements*/
    double minimal_tiling_hyperplane_rate;  /**<The minimal tiling hyperplane rate necessary to aggregate 2 statements*/

    double minimal_rate;        /**<The minimal value the weighted average of all rates to aggregate 2 statements */
    double reuse_weight;        /**<The weight of the reuse rate in the weighted average */
    double parallelism_weight;  /**<The weight of the parallelism rate in the weighted average */
    double vectorization_weight;  /**<The weight of the vectorization rate in the weighted average */
    double tiling_hyperplane_weight;  /**<The weight of the tiling hyperplane rate in the weighted average */

    enum substrate_aggregation_strategy aggregation_strategy; /**< The aggregation strategy used during optimization */
};

/**
 * An global instance of a substrate_options to be accessed everywhere we need to get
 * the options.
 */
extern struct substrate_options g_substrate_options;

struct substrate_options substrate_options_default();
void substrate_options_init(
        int argc,
        char** argv);
void substrate_print_help(
        char *program_name,
        FILE *output);
void substrate_option_terminate();


#endif
