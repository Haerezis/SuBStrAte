#pragma scop
for(i=1 ; i<N ; i++)
    A[i] = A[i-1] + 42;
#pragma endscop
