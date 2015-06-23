#include "substrate/adjacency_matrix.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct substrate_adj_matrix * substrate_adj_matrix_new(unsigned int size)
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


void substrate_adj_matrix_free(struct substrate_adj_matrix * mat)
{
    for (unsigned int i=0 ; i<mat->allocated_size ; i++)
    {
        free(mat->val[i]);
    }
    free(mat->val);
    free(mat);
}

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

void substrate_adj_matrix_remove_vertex(
        struct substrate_adj_matrix * mat,
        unsigned int vertex_index)
{
    if (vertex_index < mat->size)
    {
        free(mat->val[vertex_index]);
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


void substrate_adj_matrix_print(struct substrate_adj_matrix * mat)
{
    for(unsigned int i = 0; i<mat->size ; i++)
    {
        printf("%d | ",i);
        for(unsigned int j = 0; j<i ; j++)
        {
            printf("%.2f ",mat->val[i][j]);
        }
        printf("\n");
    }
    printf("\n\n");
}


/**
 * @brief Return the max value (with the vertices index) of an adjacency matrix.
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
    for(unsigned int i = 0; i<mat->size ; i++)
    {
        for(unsigned int j = 0 ; j<i ; j++)
        {
            if(mat->val[i][j] > *value)
            {
                *value = mat->val[i][j];
                *vertex_index1 = j;
                *vertex_index2 = i;
            }
        }
    }
}
