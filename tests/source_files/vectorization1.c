#pragma scop
for (i = 0; i < N; i++)
{
    a[i+3] = b[i+1] + c[i+2];
}
#pragma endscop
