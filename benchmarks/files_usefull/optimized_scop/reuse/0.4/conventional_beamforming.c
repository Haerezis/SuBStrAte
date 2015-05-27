#include <omp.h>
#include <math.h>
#define ceild(n,d)  ceil(((double)(n))/((double)(d)))
#define floord(n,d) floor(((double)(n))/((double)(d)))
#define max(x,y)    ((x) > (y)? (x) : (y))
#define min(x,y)    ((x) < (y)? (x) : (y))

/* 
 * File:   main.c
 * Author: joseph
 *
 * Created on April 3, 2013, 2:59 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include <float.h>

#define hnc_unused (void)

#define snr 100
#define wave_lenght 0.04
#define dist_sensor 0.02
#define nb_sensor 1000
#define nb_theta  1000
#define nb_snapshot 1000

void initialize_radar(double theta_radar[nb_theta], double matrice_real[nb_theta][nb_sensor], double matrice_img[nb_theta][nb_sensor]) {
    int i;
    for (i = 0; i < nb_theta; i++) {
        theta_radar[i] = ((double) (2.0 * M_PI * dist_sensor) / wave_lenght) * sin((-90. + i * 180. / nb_theta)*(double) (M_PI / 180.));
        //printf("theta_radar = %f\n", theta_radar[i]);
    }
    int j;
    for (i = 0; i < nb_theta; i++)
        for (j = 0; j < nb_sensor; j++) {
            matrice_real[i][j] = (double) cos(j * theta_radar[i]) / nb_sensor;
            matrice_img[i][j] = (double) -sin(j * theta_radar[i]) / nb_sensor;
            //printf("matrice = (%f,%f)\n", matrice_real[i][j], matrice_img[i][j]);
        }

}

int calculate_output(double aux_real[nb_theta], double aux_img[nb_theta], double matrice_real[nb_theta][nb_sensor], double matrice_img[nb_theta][nb_sensor], double snapshot_real[nb_sensor], double snapshot_img[nb_sensor]) {

    int i, j, t;
    double val, t_val;

    /*  for(i=0;i<nb_sensor;i++){
          printf("%lf ** %lf\n",snapshot_real[i],snapshot_img[i]);
      }*/

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
  int t1, t2, t3, t4;
 int lb, ub, lbp, ubp, lb2, ub2;
 register int lbv, ubv;
/* Start of CLooG code */
lbp=0;
ubp=nb_theta-1;
#pragma omp parallel for private(lbv,ubv,t3,t4)
for (t2=lbp;t2<=ubp;t2++) {
  aux_real[t2] = 0;;
  aux_img[t2] = 0;;
}
if (nb_sensor >= 1) {
  lbp=0;
  ubp=nb_theta-1;
#pragma omp parallel for private(lbv,ubv,t3,t4)
  for (t2=lbp;t2<=ubp;t2++) {
    for (t3=0;t3<=nb_sensor-1;t3++) {
      aux_real[t2] += snapshot_real[t3] * matrice_real[t2][t3] - snapshot_img[t3] * matrice_img[t2][t3]; aux_img[t2] += snapshot_img[t3] * matrice_real[t2][t3] + snapshot_real[t3] * matrice_img[t2][t3];;
    }
  }
}
t_val = DBL_MIN;;
t = 0;;
for (t2=0;t2<=nb_theta-1;t2++) {
  val = aux_real[t2] * aux_real[t2] + aux_img[t2] * aux_img[t2];;
  t = (val >= t_val) ? (t_val = val, t2) : t;;
}
/* End of CLooG code */

    return t;


}

void calculate_snapshot(double snapshot_real[nb_snapshot][nb_sensor], double snapshot_img[nb_snapshot][nb_sensor]) {
    double strengh, w;
    int i, j;
    strengh = pow(10, (snr / 10));
    for (i = 0; i < nb_snapshot; i++) {
        double theta = (((double) rand() / (double) RAND_MAX)) * M_PI - M_PI / 2;
        w = (2 * M_PI * dist_sensor * sin(theta)) / wave_lenght;
        for (j = 0; j < nb_sensor; j++) {
            snapshot_real[i][j] = cos(j * w) * strengh;
            snapshot_img[i][j] = sin(j * w) * strengh;
            //        printf("RRRR: %d *** %lf\n",i,snapshot_real[i][j]);
            //printf("snapshot = (%f,%f)\n", snapshot_real[i][j], snapshot_img[i][j]);
        }
    }
}

int main() {

    struct timeval t0, t1;
    int temp, g, i;
    double* y = (double*) malloc(sizeof (double)*nb_snapshot);
    //double mod1, mod2;
    int elapsed;

    //preparation du radar 
    double *matrice_real = (double*) malloc(sizeof (double) * nb_sensor * nb_theta);
    double *matrice_img = (double*) malloc(sizeof (double) * nb_sensor * nb_theta);
    double *theta_radar = (double*) malloc(sizeof (double) * nb_theta);
    double *aux_real = (double*) malloc(sizeof (double)*nb_theta);
    double *aux_img = (double*) malloc(sizeof (double)*nb_theta);
    double *snapshot_real = (double*) malloc(sizeof (double)*nb_sensor * nb_snapshot);
    double *snapshot_img = (double*) malloc(sizeof (double)*nb_sensor * nb_snapshot);

    initialize_radar(theta_radar, (double(*)[nb_sensor])matrice_real, (double(*)[nb_sensor]) matrice_img);
    calculate_snapshot((double(*)[nb_sensor])snapshot_real, (double(*)[nb_sensor])snapshot_img);

    gettimeofday(&t0, 0);
    
    for (g = 0; g < nb_snapshot; g++) {



        /*        double* t=(double(*))snapshot_real + nb_sensor*g;
                for(i=0;i<nb_sensor;i++)
                printf("%d *** %lf\n",g,t[i]);*/

        temp = calculate_output(aux_real, aux_img, (double(*)[nb_sensor])matrice_real, (double(*)[nb_sensor])matrice_img, (double(*))snapshot_real + nb_sensor*g, (double(*))snapshot_img + nb_sensor * g);

        /*        temp = 0;
                for (i = 1; i < nb_theta; i++) {
                    mod1 = pow(aux_real[i], 2) + pow(aux_img[i], 2);
                    mod2 = pow(aux_real[temp], 2) + pow(aux_img[temp], 2);
                    temp = (mod1 >= mod2) ? i : temp;
                    //       mod2 = (mod1 >= mod2) ? mod2 = pow(aux_real[temp], 2) + pow(aux_img[temp], 2) : mod2;
                    if (mod1 >= mod2) {
                        temp = i;
                        mod2 = pow(aux_real[temp], 2) + pow(aux_img[temp], 2);
                    }
                }*/

        y[g] = (-90. + temp * 180. / nb_theta);
    }

    gettimeofday(&t1, 0);

//     for (i = 0; i < nb_snapshot; i++) {
//         printf("%lf\n", y[i]);
//     }
    elapsed = (t1.tv_sec - t0.tv_sec) * 1000000 + t1.tv_usec - t0.tv_usec;
    printf("Elapsed time: %.3f ms\n", ((float) elapsed) / (1000.));

    return (EXIT_SUCCESS);
}
