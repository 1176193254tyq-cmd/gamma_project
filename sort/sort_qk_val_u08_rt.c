/* MWDSP_Sort_Qk_Val_U08 Function to sort an input array of real 
 * uint8_T for Sort block in DSP System Toolbox
 *
 *  Implement Quicksort in-place sort-by-value algorithm
 *
 *  Copyright 1995-2016 The MathWorks, Inc.
 */

#ifdef MW_DSP_RT
#include "src/dspsrt_rt.h"
#else
#include "dspsrt_rt.h"
#endif

static boolean_T findPivot(uint8_T *dataArray, int_T iQS, int_T jQS, int_T *pivot )
{
    int_T   mid = (iQS+jQS)>>1;
    int_T   kQS;
    uint8_T  a, b, c;

    if(dataArray[iQS] > dataArray[mid]) {
        uint8_T    tmp = dataArray[iQS];
        dataArray[iQS] = dataArray[mid];
        dataArray[mid] = tmp;
    }
    if(dataArray[iQS] > dataArray[jQS]) {
        uint8_T    tmp = dataArray[iQS];
        dataArray[iQS] = dataArray[jQS];
        dataArray[jQS] = tmp;
    }
    if(dataArray[mid] > dataArray[jQS]) {
        uint8_T    tmp = dataArray[mid];
        dataArray[mid] = dataArray[jQS];
        dataArray[jQS] = tmp;
    }

    a = dataArray[iQS];
    b = dataArray[mid];
    c = dataArray[jQS];

    if (a < b) {   
        *pivot = mid;
        return((boolean_T)1);
    }
    if (b < c) {
        *pivot = jQS;
        return((boolean_T)1);
    }
    for (kQS=iQS+1; kQS <= jQS; kQS++) {
        uint8_T d = dataArray[kQS];
        if (d != a) {
          *pivot = (d < a) ? iQS : kQS;
          return((boolean_T)1);
        }
    }
    return((boolean_T)0);

}

static int_T partition(uint8_T *dataArray, int_T iQS, int_T jQS, int_T pivot)
{
    uint8_T pval = dataArray[pivot];

    while (iQS <= jQS) {
        while(dataArray[iQS] < pval) {
            ++iQS;
        }
        while(dataArray[jQS] >= pval) {
            --jQS;
        }
        if (iQS<jQS) {
            uint8_T    tmp = dataArray[iQS];
            dataArray[iQS] = dataArray[jQS];
            dataArray[jQS] = tmp;
            ++iQS; 
            --jQS;
        }
    }
    return(iQS);
}

/* The recursive quicksort routine: */
LIBMW_SRC_API void MWDSP_Sort_Qk_Val_U08(uint8_T *dataArray, int_T iQS, int_T jQS)
{
    int_T pivot;
    if (findPivot(dataArray, iQS, jQS, &pivot)) {
        int_T kQS = partition(dataArray, iQS, jQS, pivot);
        MWDSP_Sort_Qk_Val_U08(dataArray, iQS, kQS-1);
        MWDSP_Sort_Qk_Val_U08(dataArray, kQS, jQS);
    }
}


/* [EOF] */
