#pragma scop
for(i=0 ; i<N1 ; i++)
    for(j=0 ; j<N2 ; j++)
        A[j+1] = (A[j] + A[j+1] + A[j+2]) / 3.0;
#pragma endscop
