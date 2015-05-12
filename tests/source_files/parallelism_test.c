#pragma scop
for(i=0 ; i<N ; i++)
{
    for(j=1 ; j<N ; j++)
    {
        A[i][j] = 42;
        B[i][j] = B[i][j-1];
        B[i][j] = B[i-1][j];
    }
    C[i] = D[i-1];
}
#pragma endscop
