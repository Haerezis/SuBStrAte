#pragma scop
t = 0;                                                      /* S0 */
t_val = DBL_MIN;                                            /* S1 */
for (i = 0; i < N; i++) {           /* L1 */
    a_i[i] = 0;                                             /* S2 */
    a_r[i] = 0;                                             /* S3 */
    for (j = 0; j < M; j++) {       /* L2 */
        a_r[i] += s_r[j] * m_r[i][j] - s_i[j] * m_i[i][j];  /* S4 */
        a_i[i] += s_i[j] * m_r[i][j] + s_r[j] * m_i[i][j];  /* S5 */
    }
    val = a_r[i] * a_r[i] + a_i[i] * a_i[i];                /* S6 */
    t = (val >= t_val)? (t_val = val, i) : t;               /* S7 */
}
#pragma endscop
