/* MWDSP_Sort_Ins_Val_U32 Function to sort an input array of real
 * uint32_T for Sort block in DSP System Toolbox
 *
 *  Implement Insertion sort-by-index algorithm
 *
 *  Copyright 1995-2016 The MathWorks, Inc.
 */

#ifdef MW_DSP_RT
#include "src/dspsrt_rt.h"
#else
#include "dspsrt_rt.h"
#endif

LIBMW_SRC_API void MWDSP_Sort_Ins_Idx_U32(const uint32_T *a, uint32_T *idxArray, int_T n)
{
    uint32_T i0 = idxArray[0];
    uint32_T t0 = a[i0];
    int_T iSI;
    for (iSI=1; iSI<n; iSI++) {
        uint32_T i1 = idxArray[iSI];
        uint32_T t1 = a[i1];
        if (t0 > t1) {
            int_T jSI;
            idxArray[iSI] = i0;
            for (jSI=iSI-1; jSI>0; jSI--) {
                uint32_T i2 = idxArray[jSI-1];
                uint32_T t2 = a[i2];
                if (t2 > t1) {
                    idxArray[jSI] = i2;
                } else {
                    break;
                }
            }
            idxArray[jSI] = i1;
        } else {
            t0 = t1;
            i0 = i1;
        }
    }
}

/* [EOF] */
