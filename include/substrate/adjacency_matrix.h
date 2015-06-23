#ifndef __SUBSTRATE__ADJACENCY_MATRIX_H
#define __SUBSTRATE__ADJACENCY_MATRIX_H



struct substrate_adj_matrix {
    double **val;
    unsigned int size;
    unsigned int allocated_size;
};

struct substrate_adj_matrix * substrate_adj_matrix_new(unsigned int size);
void substrate_adj_matrix_free(struct substrate_adj_matrix * mat);

double substrate_adj_matrix_get_edge(
        struct substrate_adj_matrix * mat,
        unsigned int vertex_index1,
        unsigned int vertex_index2);
void substrate_adj_matrix_set_edge(
        struct substrate_adj_matrix * mat,
        unsigned int vertex_index1,
        unsigned int vertex_index2,
        double value);
void substrate_adj_matrix_remove_vertex(
        struct substrate_adj_matrix * mat,
        unsigned int vertex_index);

void substrate_adj_matrix_max_edge(
        struct substrate_adj_matrix * mat,
        double * value,
        unsigned int * vertex_index1,
        unsigned int * vertex_index2);


void substrate_adj_matrix_print(struct substrate_adj_matrix * mat);
#endif
