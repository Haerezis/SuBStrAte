#pragma scop
for (i = 0; i < N; i++)
{
    for (j = 0; j < N; j++)
    {
        a[i+3][j] = b[i+1][j] + c[i+2][j];
    }
}
#pragma endscop
