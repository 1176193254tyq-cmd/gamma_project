/* QSRT_QID_PARTITION_D_RT Helper functions for Sort block in DSP System Toolbox, data-type = double precision.
 *
 *  Copyright 1995-2016 The MathWorks, Inc.
 */

#if (!defined(INTEGER_CODE) || !INTEGER_CODE)

#ifdef MW_DSP_RT
#include "src/dspsrt_rt.h"
#else
#include "dspsrt_rt.h"
#endif

LIBMW_SRC_API int_T MWDSP_SrtQidPartitionD(const real_T *qid_array, int_T *qid_index,
                           int_T iQS, int_T jQS, int_T pivot )
{
    real_T pval = *(qid_array + *(qid_index + pivot));

    while (iQS <= jQS) {
        while( *( qid_array + *(qid_index+iQS) ) <  pval) {
            ++iQS;
        }
        while( *( qid_array + *(qid_index+jQS) ) >= pval) {
            --jQS;
        }
        if (iQS<jQS) {
            qid_Swap(iQS,jQS)
            ++iQS; --jQS;
        }
    }
    return(iQS);
}

#endif /* !INTEGER_CODE */

/* [EOF] srt_qid_partition_d_rt.c */

