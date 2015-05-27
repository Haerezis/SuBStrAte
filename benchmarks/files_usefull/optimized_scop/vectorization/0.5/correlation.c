#include <omp.h>
#include <math.h>
#define ceild(n,d)  ceil(((double)(n))/((double)(d)))
#define floord(n,d) floor(((double)(n))/((double)(d)))
#define max(x,y)    ((x) > (y)? (x) : (y))
#define min(x,y)    ((x) < (y)? (x) : (y))

/**
 * This version is stamped on Apr. 14, 2015
 *
 * Contact:
 *   Louis-Noel Pouchet <pouchet.ohio-state.edu>
 *   Tomofumi Yuki <tomofumi.yuki.fr>
 *
 * Web address: http://polybench.sourceforge.net
 */
/* correlation.c: this file is part of PolyBench/C */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <math.h>

/* Include polybench common header. */
#include <polybench.h>

/* Include benchmark-specific header. */
#include "correlation.h"


/* Array initialization. */
static
void init_array (int m,
		 int n,
		 DATA_TYPE *float_n,
		 DATA_TYPE POLYBENCH_2D(data,N,M,n,m))
{
  int i, j;

  *float_n = (DATA_TYPE)N;

  for (i = 0; i < N; i++)
    for (j = 0; j < M; j++)
      data[i][j] = (DATA_TYPE)(i*j)/M + i;
    
}


/* DCE code. Must scan the entire live-out data.
   Can be used also to check the correctness of the output. */
static
void print_array(int m,
		 DATA_TYPE POLYBENCH_2D(corr,M,M,m,m))

{
  int i, j;

  POLYBENCH_DUMP_START;
  POLYBENCH_DUMP_BEGIN("corr");
  for (i = 0; i < m; i++)
    for (j = 0; j < m; j++) {
      if ((i * m + j) % 20 == 0) fprintf (POLYBENCH_DUMP_TARGET, "\n");
      fprintf (POLYBENCH_DUMP_TARGET, DATA_PRINTF_MODIFIER, corr[i][j]);
    }
  POLYBENCH_DUMP_END("corr");
  POLYBENCH_DUMP_FINISH;
}


/* Main computational kernel. The whole function will be timed,
   including the call and return. */
static
void kernel_correlation(int m, int n,
			DATA_TYPE float_n,
			DATA_TYPE POLYBENCH_2D(data,N,M,n,m),
			DATA_TYPE POLYBENCH_2D(corr,M,M,m,m),
			DATA_TYPE POLYBENCH_1D(mean,M,m),
			DATA_TYPE POLYBENCH_1D(stddev,M,m))
{
  int i, j, k;

  DATA_TYPE eps = SCALAR_VAL(0.1);


/* Copyright (C) 1991-2014 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <http://www.gnu.org/licenses/>.  */
/* This header is separate from features.h so that the compiler can
   include it implicitly at the start of every compilation.  It must
   not itself include <features.h> or any other header that includes
   <features.h> because the implicit include comes before any feature
   test macros that may be defined in a source file before it first
   explicitly includes a system header.  GCC knows the name of this
   header in order to preinclude it.  */
/* glibc's intent is to support the IEC 559 math functionality, real
   and complex.  If the GCC (4.9 and later) predefined macros
   specifying compiler intent are available, use them to determine
   whether the overall intent is to support these features; otherwise,
   presume an older compiler has intent to support these features and
   define these macros by default.  */
/* wchar_t uses ISO/IEC 10646 (2nd ed., published 2011-03-15) /
   Unicode 6.0.  */
/* We do not support C11 <threads.h>.  */
  int t1, t2, t3, t4, t5;
 int lb, ub, lbp, ubp, lb2, ub2;
 register int lbv, ubv;
/* Start of CLooG code */
corr[_PB_M-1][_PB_M-1] = SCALAR_VAL(1.0);;
lbp=0;
ubp=_PB_M-2;
#pragma omp parallel for private(lbv,ubv,t3,t4,t5)
for (t2=lbp;t2<=ubp;t2++) {
  for (t3=t2+1;t3<=_PB_M-1;t3++) {
    corr[t2][t3] = SCALAR_VAL(0.0);;
  }
}
lbp=0;
ubp=_PB_M-2;
#pragma omp parallel for private(lbv,ubv,t3,t4,t5)
for (t2=lbp;t2<=ubp;t2++) {
  corr[t2][t2] = SCALAR_VAL(1.0);;
  stddev[t2] = SCALAR_VAL(0.0);;
  mean[t2] = SCALAR_VAL(0.0);;
}
if (_PB_M >= 1) {
  stddev[(_PB_M-1)] = SCALAR_VAL(0.0);;
  mean[(_PB_M-1)] = SCALAR_VAL(0.0);;
}
if (_PB_N >= 1) {
  lbp=0;
  ubp=_PB_M-1;
#pragma omp parallel for private(lbv,ubv,t3,t4,t5)
  for (t2=lbp;t2<=ubp;t2++) {
    for (t3=0;t3<=_PB_N-1;t3++) {
      mean[t2] += data[t3][t2];;
    }
  }
}
lbp=0;
ubp=_PB_M-1;
#pragma omp parallel for private(lbv,ubv,t3,t4,t5)
for (t2=lbp;t2<=ubp;t2++) {
  mean[t2] /= float_n;;
}
if (_PB_N >= 1) {
  lbp=0;
  ubp=_PB_M-1;
#pragma omp parallel for private(lbv,ubv,t3,t4,t5)
  for (t2=lbp;t2<=ubp;t2++) {
    for (t3=0;t3<=_PB_N-1;t3++) {
      stddev[t2] += (data[t3][t2] - mean[t2]) * (data[t3][t2] - mean[t2]);;
      data[t3][t2] -= mean[t2];;
    }
  }
}
lbp=0;
ubp=_PB_M-1;
#pragma omp parallel for private(lbv,ubv,t3,t4,t5)
for (t2=lbp;t2<=ubp;t2++) {
  stddev[t2] /= float_n;;
  stddev[t2] = SQRT_FUN(stddev[t2]);;
  stddev[t2] = stddev[t2] <= eps ? SCALAR_VAL(1.0) : stddev[t2];;
}
if (_PB_M >= 1) {
  lbp=0;
  ubp=_PB_N-1;
#pragma omp parallel for private(lbv,ubv,t3,t4,t5)
  for (t2=lbp;t2<=ubp;t2++) {
    for (t3=0;t3<=_PB_M-1;t3++) {
      data[t2][t3] /= SQRT_FUN(float_n) * stddev[t3];;
    }
  }
}
if (_PB_N >= 1) {
  lbp=0;
  ubp=_PB_M-2;
#pragma omp parallel for private(lbv,ubv,t3,t4,t5)
  for (t2=lbp;t2<=ubp;t2++) {
    for (t3=t2+1;t3<=_PB_M-1;t3++) {
      for (t4=0;t4<=_PB_N-1;t4++) {
        corr[t2][t3] += (data[t4][t2] * data[t4][t3]);;
      }
    }
  }
}
lbp=0;
ubp=_PB_M-2;
#pragma omp parallel for private(lbv,ubv,t3,t4,t5)
for (t2=lbp;t2<=ubp;t2++) {
  for (t3=t2+1;t3<=_PB_M-1;t3++) {
    corr[t3][t2] = corr[t2][t3];;
  }
}
/* End of CLooG code */

}


int main(int argc, char** argv)
{
  /* Retrieve problem size. */
  int n = N;
  int m = M;

  /* Variable declaration/allocation. */
  DATA_TYPE float_n;
  POLYBENCH_2D_ARRAY_DECL(data,DATA_TYPE,N,M,n,m);
  POLYBENCH_2D_ARRAY_DECL(corr,DATA_TYPE,M,M,m,m);
  POLYBENCH_1D_ARRAY_DECL(mean,DATA_TYPE,M,m);
  POLYBENCH_1D_ARRAY_DECL(stddev,DATA_TYPE,M,m);

  /* Initialize array(s). */
  init_array (m, n, &float_n, POLYBENCH_ARRAY(data));

  /* Start timer. */
  polybench_start_instruments;

  /* Run kernel. */
  kernel_correlation (m, n, float_n,
		      POLYBENCH_ARRAY(data),
		      POLYBENCH_ARRAY(corr),
		      POLYBENCH_ARRAY(mean),
		      POLYBENCH_ARRAY(stddev));

  /* Stop and print timer. */
  polybench_stop_instruments;
  polybench_print_instruments;

  /* Prevent dead-code elimination. All live-out data must be printed
     by the function call in argument. */
  polybench_prevent_dce(print_array(m, POLYBENCH_ARRAY(corr)));

  /* Be clean. */
  POLYBENCH_FREE_ARRAY(data);
  POLYBENCH_FREE_ARRAY(corr);
  POLYBENCH_FREE_ARRAY(mean);
  POLYBENCH_FREE_ARRAY(stddev);

  return 0;
}
