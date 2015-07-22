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


{ int lb, ub, lbp, ubp, lb2, ub2;
hnc_unused(lb);
hnc_unused(ub);
hnc_unused(lbp);
hnc_unused(ubp);
hnc_unused(lb2);
hnc_unused(ub2);
register int lbv, ubv;
hnc_unused(lbv);
hnc_unused(ubv);
int t1, t1t, newlb_t1, newub_t1;
hnc_unused(t1);
hnc_unused(t1t);
hnc_unused(newlb_t1);
hnc_unused(newub_t1);
int t2, t2t, newlb_t2, newub_t2;
hnc_unused(t2);
hnc_unused(t2t);
hnc_unused(newlb_t2);
hnc_unused(newub_t2);
int t3, t3t, newlb_t3, newub_t3;
hnc_unused(t3);
hnc_unused(t3t);
hnc_unused(newlb_t3);
hnc_unused(newub_t3);
int t4, t4t, newlb_t4, newub_t4;
hnc_unused(t4);
hnc_unused(t4t);
hnc_unused(newlb_t4);
hnc_unused(newub_t4);
int t5, t5t, newlb_t5, newub_t5;
hnc_unused(t5);
hnc_unused(t5t);
hnc_unused(newlb_t5);
hnc_unused(newub_t5);
int t6, t6t, newlb_t6, newub_t6;
hnc_unused(t6);
hnc_unused(t6t);
hnc_unused(newlb_t6);
hnc_unused(newub_t6);
int t7, t7t, newlb_t7, newub_t7;
hnc_unused(t7);
hnc_unused(t7t);
hnc_unused(newlb_t7);
hnc_unused(newub_t7);
int t8, t8t, newlb_t8, newub_t8;
hnc_unused(t8);
hnc_unused(t8t);
hnc_unused(newlb_t8);
hnc_unused(newub_t8);
int t9, t9t, newlb_t9, newub_t9;
hnc_unused(t9);
hnc_unused(t9t);
hnc_unused(newlb_t9);
hnc_unused(newub_t9);
int t10, t10t, newlb_t10, newub_t10;
hnc_unused(t10);
hnc_unused(t10t);
hnc_unused(newlb_t10);
hnc_unused(newub_t10);

#define floord(n,d) (((n)<0) ? -((-(n)+(d)-1)/(d)) : (n)/(d))
#define ceild(n,d)  (((n)<0) ? -((-(n))/(d)) : ((n)+(d)-1)/(d))
#define max(x,y)    ((x) > (y) ? (x) : (y))
#define min(x,y)    ((x) < (y) ? (x) : (y))

lbp=0;
ubp=nb_theta-1;
#pragma omp parallel for private(lbv,ubv)
for (t2=lbp;t2<=ubp;t2++) {
  aux_real[t2] = 0;
}
if (nb_sensor >= 1) {
  lbp=0;
  ubp=nb_theta-1;
#pragma omp parallel for private(lbv,ubv)
  for (t2=lbp;t2<=ubp;t2++) {
    for (t3=0;t3<=nb_sensor-1;t3++) {
      aux_real[t2] += snapshot_real[t3] * matrice_real[t2][t3] - snapshot_img[t3] * matrice_img[t2][t3];
    }
  }
}
lbp=0;
ubp=nb_theta-1;
#pragma omp parallel for private(lbv,ubv)
for (t2=lbp;t2<=ubp;t2++) {
  aux_img[t2] = 0;
}
if (nb_sensor >= 1) {
  lbp=0;
  ubp=nb_theta-1;
#pragma omp parallel for private(lbv,ubv)
  for (t2=lbp;t2<=ubp;t2++) {
    for (t3=0;t3<=nb_sensor-1;t3++) {
      aux_img[t2] += snapshot_img[t3] * matrice_real[t2][t3] + snapshot_real[t3] * matrice_img[t2][t3];
    }
  }
}
t = 0;
t_val = DBL_MIN;
for (t2=0;t2<=nb_theta-1;t2++) {
  val = aux_real[t2] * aux_real[t2] + aux_img[t2] * aux_img[t2];
  t = (val >= t_val) ? (t_val = val, t2) : t;
}

 }

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

