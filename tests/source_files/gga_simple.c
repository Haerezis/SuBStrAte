#pragma scop
for (i=0 ; i<N ; i++)
{
  a[i] = 42;
  b[i] = aa;
  c[i] = a[i] * 2;
  d[i] = (a[i] + a[i+1]) / 2;
}
#pragma endscop
