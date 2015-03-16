#pragma scop
for(i=0 ; i<N ; i++)
    for(j=0; j<N ; j++)
        A[i][j] = 0.2 * (A[i][j] + A[i+1][j] + A[i-1][j] + A[i][j+1] + A[i][j-1]);
#pragma endscop
