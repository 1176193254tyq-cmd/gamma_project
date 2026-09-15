/* QSRT_QID_FINDPIVOT_D_RT Helper functions for Sort block in DSP System Toolbox, data-type = double precision.
 *
 *  Copyright 1995-2016 The MathWorks, Inc.
 */

#if (!defined(INTEGER_CODE) || !INTEGER_CODE)

#ifdef MW_DSP_RT
#include "src/dspsrt_rt.h"
#else
#include "dspsrt_rt.h"
#endif

LIBMW_SRC_API boolean_T MWDSP_SrtQidFindPivotD(const real_T *qid_array, int_T *qid_index,
                               int_T iQS, int_T jQS, int_T *pivot )
{
    int_T  mid = (iQS+jQS)/2;
    int_T  kQS;
    real_T a, b, c;

    qid_Order3(iQS,mid,jQS);    /* order the 3 values */
    a = *(qid_array + *(qid_index + iQS));
    b = *(qid_array + *(qid_index + mid));
    c = *(qid_array + *(qid_index + jQS));

    if (a < b) {   /* pivot will be higher of 2 values */
        *pivot = mid;
        return((boolean_T)1);
    }
    if (b < c) {
        *pivot = jQS;
        return((boolean_T)1);
    }
    for (kQS=iQS+1; kQS <= jQS; kQS++) {
        real_T d = *(qid_array + *(qid_index + kQS));
        if (d != a) {
          *pivot = (d < a) ? iQS : kQS;
          return((boolean_T)1);
        }
    }
    return((boolean_T)0);
}

#endif /* !INTEGER_CODE */

/* [EOF] srt_qid_findpivot_d_rt.c */


