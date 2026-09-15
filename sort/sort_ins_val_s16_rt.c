/* MWDSP_Sort_Ins_Val_S16 Function to sort an input array of real
 * int16_T for Sort block in DSP System Toolbox
 *
 *  Implement Insertion sort-by-value algorithm
 *
 *  Copyright 1995-2016 The MathWorks, Inc.
 */

#ifdef MW_DSP_RT
#include "src/dspsrt_rt.h"
#else
#include "dspsrt_rt.h"
#endif

/* insertion sort in-place by value */
LIBMW_SRC_API void MWDSP_Sort_Ins_Val_S16(int16_T *a, int_T n )
{
    int16_T t0 = a[0];
    int_T iSI;
    for (iSI=1; iSI<n; iSI++) {
        int16_T t1 = a[iSI];
        if (t0 > t1) {
            int_T jSI;
            a[iSI] = t0;
            for (jSI=iSI-1; jSI>0; jSI--) {
                int16_T t2 = a[jSI-1];
                if (t2 > t1) {
                    a[jSI] = t2;
                } else {
                    break;
                }
            }
            a[jSI] = t1;
        } else {
            t0 = t1;
        }
    }
}

/* [EOF] */
