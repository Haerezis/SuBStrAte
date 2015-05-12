#pragma scop
for(i=0 ; i<N ; i++)
    for(j=0 ; j<(N-1) ; j++)
    {
        A[i][j] = 42;
        A[i][j+1] = 42;
    }
#pragma endscop
