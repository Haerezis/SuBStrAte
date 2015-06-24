#ifndef __SUBSTRATE__OPTIMIZATION_H
#define __SUBSTRATE__OPTIMIZATION_H

#include <osl/osl.h>

#include "substrate/statement_profile.h"
#include "substrate/adjacency_matrix.h"


double substrate_weighted_rate(
        double reuse_rate,
        double parallelism_rate,
        double vectorization_rate,
        double tiling_hyperplane_rate);
double substrate_similarity_rate(
        struct substrate_statement_profile * stmt_profile1,
        struct substrate_statement_profile * stmt_profile2);
struct osl_scop * substrate_optimize(struct osl_scop*);
void substrate_successive_statements_optimization(struct osl_scop *);
void substrate_greedy_graph_optimization(struct osl_scop * profiled_scop);


void substrate_move_statement(
        struct osl_scop * scop,
        unsigned int src_stmt_index,
        unsigned int dst_stmt_index,
        bool before);
bool substrate_is_stmt_moving_legal(
        struct osl_scop * scop,
        unsigned int src_stmt_index,
        unsigned int dst_stmt_index,
        bool before);
void substrate_update_adj_matrix(
        struct osl_scop * profiled_scop,
        struct substrate_adj_matrix * adj_mat,
        unsigned int stmt_index);


#endif
