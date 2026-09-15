#ifndef PF_CONVERT_FINAL_H
#define PF_CONVERT_FINAL_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

float f_abs(float x);
int f_sgn(float x);
float f_clamp(float x, float lo, float hi);

bool P_PF_to_S_pf_cmd(float P, float PF, float* S_cmd, float* pf_cmd);

void P_PF_read_to_S_pf_out(float P_read, float Q_read, float PF_read,
                           float* S_out, float* pf_out);
bool within_apparent_limit(float P, float Q, float Smax);
#ifdef __cplusplus
}
#endif

#endif