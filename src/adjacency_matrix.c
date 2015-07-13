#include "substrate/adjacency_matrix.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Allocate and initialize a substrate_adj_matrix.
 * The matrix contained int the structure is a only triangular :
 * o
 * oo
 * ooo
 *
 * @param[in] size The size of the matrix.
 *
 * @return The allocated and initialized substrate_adj_matrix.
 */
struct substrate_adj_matrix * substrate_adj_matrix_malloc(unsigned int size)
{
    struct substrate_adj_matrix * res = NULL;

    res = (struct substrate_adj_matrix*)malloc(sizeof(struct substrate_adj_matrix));
    res->size = size;
    res->allocated_size = size;

    res->val = (double**) malloc(size * sizeof(double*));
    for (unsigned int i=0 ; i<size ; i++)
    {
        res->val[i] = (double*) malloc(i * sizeof(double));
        for(unsigned int j = 0 ; j<i ; j++)
        {
            res->val[i][j] = 0.0;
        }
    }
    
    return res;
}


/**
 * @brief Free the structure of a substrate_adj_matrix structure.
 *
 * @param[inout] mat A pointer to the structure that will be freed.
 */
void substrate_adj_matrix_free(struct substrate_adj_matrix * mat)
{
    for (unsigned int i=0 ; i<mat->allocated_size ; i++)
    {
        free(mat->val[i]);
    }
    free(mat->val);
    free(mat);
}


/**
 * @brief Get the weight of the edge between 2 vertices.
 *
 * @param[in] mat A pointer to the adjacency matrix where is taken the value of the edge.
 * @param[in] vertex_index1 The index of the first vertex.
 * @param[in] vertex_index2 The index of the second vertex.
 *
 * @return The weight of the edge.
 */
double substrate_adj_matrix_get_edge(
        struct substrate_adj_matrix * mat,
        unsigned int vertex_index1,
        unsigned int vertex_index2)
{
    double res = 0.0;
    if (vertex_index1 < vertex_index2)
    {
        unsigned int tmp = vertex_index1;
        vertex_index1 = vertex_index2;
        vertex_index2 = tmp;
    }
    if ((vertex_index1 != vertex_index2) && (vertex_index1 < mat->size))
    {
        res = mat->val[vertex_index1][vertex_index2];
    }

    return res;
}

/**
 * @brief Set the weight of the edge between 2 vertices.
 *
 * @param[inout] mat A pointer to the adjacency matrix where is set the value of the edge.
 * @param[in] vertex_index1 The index of the first vertex.
 * @param[in] vertex_index2 The index of the second vertex.
 * @param[in] value The value that will be set for the edge beteen vertex_index1
 * and vertex_index2.
 */
void substrate_adj_matrix_set_edge(
        struct substrate_adj_matrix * mat,
        unsigned int vertex_index1,
        unsigned int vertex_index2,
        double value)
{
    if (vertex_index1 < vertex_index2)
    {
        unsigned int tmp = vertex_index1;
        vertex_index1 = vertex_index2;
        vertex_index2 = tmp;
    }
    if ((vertex_index1 != vertex_index2) && (vertex_index1 < mat->size))
    {
        mat->val[vertex_index1][vertex_index2] = value;
    }
}

/**
 * @brief Remove a vertex from the adjacency matrix (remove a line and column).
 *
 * @param[inout] mat A pointer to the adjacency matrix that will be modified.
 * @param[in] vertex_index The index of the vertex that will be removed.
 */
void substrate_adj_matrix_remove_vertex(
        struct substrate_adj_matrix * mat,
        unsigned int vertex_index)
{
    if (vertex_index < mat->size)
    {
        free(mat->val[vertex_index]);
        mat->allocated_size--;
        if(vertex_index < (mat->size-1))
        {
            memmove(
                    &mat->val[vertex_index],
                    &mat->val[vertex_index+1],
                    sizeof(double*) * (mat->size - vertex_index - 1));
        }
        mat->size--;
        for(unsigned int i = vertex_index+1 ; i<mat->size ; i++)
        {
            memmove(
                    &mat->val[i][vertex_index],
                    &mat->val[i][vertex_index + 1],
                    sizeof(double) * (mat->size - vertex_index - 1));
        }
    }
}


/**
 * @brief Dump the content of a substrate_adj_matrix to a output stream.
 * (a file, stdout or stderr).
 *
 * @param[inout] output_stream The output stream where will be printed the matrix.
 * @param[in] mat A pointer to the adjacency matrix that will be printed.
 */
void substrate_adj_matrix_print(
    FILE * output_stream,
    struct substrate_adj_matrix * mat)
{
    for(unsigned int i = 0; i<mat->size ; i++)
    {
        fprintf(output_stream, "%d | ",i);
        for(unsigned int j = 0; j<i ; j++)
        {
            fprintf(output_stream, "%.2f ",mat->val[i][j]);
        }
        fprintf(output_stream, "\n");
    }
    fprintf(output_stream, "\n\n");
}


/**
 * @brief Return the max value (with the vertices index) of an adjacency matrix's edges.
 *
 * @param[in] mat The adjacency matrix.
 * @param[out] value pointer to where will be stored the max value.
 * @param[out] vertex_index1 pointer to where will be stored the index of the 1st vertex.
 * @param[out] vertex_index2 pointer to where will be stored the index of the 2nd vertex.
 */
void substrate_adj_matrix_max_edge(
        struct substrate_adj_matrix * mat,
        double * value,
        unsigned int * vertex_index1,
        unsigned int * vertex_index2)
{
    double val = mat->val[0][0];
    unsigned int max_i = 0, max_j = 0;

    for(unsigned int i = 0; i<mat->size ; i++)
    {
        for(unsigned int j = 0 ; j<i ; j++)
        {
            if(mat->val[i][j] > val)
            {
                val = mat->val[i][j];
                max_j = j;
                max_i = i;
            }
        }
    }
    *value = val;
    *vertex_index1 = max_j;
    *vertex_index2 = max_i;
}
