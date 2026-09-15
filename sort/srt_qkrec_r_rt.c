/* QSRT_R_RT Function to sort an input array of real singles for Sort block in DSP System Toolbox.
 *
 * Implement Quicksort algorithm using indices (qid)
 * Note: this algorithm is different from MATLAB's sorting
 * for complex values with same magnitude.
 *
 * Sorts an array of singles based on the "Quicksort" algorithm,
 * using an index vector rather than the data itself.
 *
 *  Copyright 1995-2016 The MathWorks, Inc.
 */

#if (!defined(INTEGER_CODE) || !INTEGER_CODE)

#ifdef MW_DSP_RT
#include "src/dspsrt_rt.h"
#else
#include "dspsrt_rt.h"
#endif

/* The recursive quicksort routine: */
LIBMW_SRC_API void MWDSP_SrtQkRecR(const real32_T *qid_array, int_T *qid_index,
                        int_T iQS, int_T jQS)
{
    int_T pivot;
    if (MWDSP_SrtQidFindPivotR(qid_array, qid_index, iQS, jQS, &pivot)) {
        int_T kQS = MWDSP_SrtQidPartitionR(qid_array, qid_index, iQS, jQS, pivot);
        MWDSP_SrtQkRecR(qid_array, qid_index, iQS, kQS-1);
        MWDSP_SrtQkRecR(qid_array, qid_index, kQS, jQS);
    }
}

#endif /* !INTEGER_CODE */


/* [EOF] srt_qkrec_r_rt.c */
