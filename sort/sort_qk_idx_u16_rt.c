/* MWDSP_Sort_Qk_Idx_U16 Function to sort an input array of real 
 * uint16_T for Sort block in DSP System Toolbox.
 *
 * Implement Quicksort algorithm using indices
 * Note: this algorithm is different from MATLAB's sorting
 * for complex values with same magnitude.
 *
 * Sorts an array of singles based on the "Quicksort" algorithm,
 * using an index vector rather than the data itself.
 *
 *  Copyright 1995-2016 The MathWorks, Inc.
 */

#ifdef MW_DSP_RT
#include "src/dspsrt_rt.h"
#else
#include "dspsrt_rt.h"
#endif

static boolean_T findPivot(const uint16_T *dataArray, uint32_T *idxArray,
                               int_T iQS, int_T jQS, int_T *pivot )
{
    int_T  mid = (iQS+jQS)/2;
    int_T  kQS;
    uint16_T a, b, c;

    qsortIdxOrder3(iQS,mid,jQS);    /* order the 3 values */
    a = *(dataArray + *(idxArray + iQS));
    b = *(dataArray + *(idxArray + mid));
    c = *(dataArray + *(idxArray + jQS));

    if (a < b) {   /* pivot will be higher of 2 values */
        *pivot = mid;
        return((boolean_T)1);
    }
    if (b < c) {
        *pivot = jQS;
        return((boolean_T)1);
    }
    for (kQS=iQS+1; kQS <= jQS; kQS++) {
        uint16_T d = *(dataArray + *(idxArray + kQS));
        if (d != a) {
          *pivot = (d < a) ? iQS : kQS;
          return((boolean_T)1);
        }
    }
    return((boolean_T)0);
}

static int_T partition(const uint16_T *dataArray, uint32_T *idxArray,
                           int_T iQS, int_T jQS, int_T pivot )
{
    uint16_T pval = *(dataArray + *(idxArray + pivot));

    while (iQS <= jQS) {
        while( *( dataArray + *(idxArray+iQS) ) <  pval) {
            ++iQS;
        }
        while( *( dataArray + *(idxArray+jQS) ) >= pval) {
            --jQS;
        }
        if (iQS<jQS) {
            qsortIdxSwap(iQS,jQS)
            ++iQS; --jQS;
        }
    }
    return(iQS);
}

/* The recursive quicksort routine: */
LIBMW_SRC_API void MWDSP_Sort_Qk_Idx_U16(const uint16_T *dataArray, uint32_T *idxArray,
                        int_T iQS, int_T jQS )
{
    int_T pivot;
    if (findPivot(dataArray, idxArray, iQS, jQS, &pivot)) {
        int_T kQS = partition(dataArray, idxArray, iQS, jQS, pivot);
        MWDSP_Sort_Qk_Idx_U16(dataArray, idxArray, iQS, kQS-1);
        MWDSP_Sort_Qk_Idx_U16(dataArray, idxArray, kQS, jQS);
    }
}


/* [EOF] */
