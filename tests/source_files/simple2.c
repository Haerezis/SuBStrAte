#pragma scop
for(i=1 ; i<N ; i++)
{
    B[i] = 42;
    for(j=1; j<N; j++)
    {
        A[i][j] = A[i-1][j] + 42;
    }
}
#pragma endscop
