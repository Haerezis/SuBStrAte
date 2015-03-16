#pragma scop
for(i=0; i<N ; i++)
    for(j=0; j<N ; j++)
        A[i][j] = B[i][j-1] + B[i][j] + B[i][j+1];
#pragma endscop
