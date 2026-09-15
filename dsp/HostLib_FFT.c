/**
 * @file HostLib_FFT.c
 * @brief Helper for FFT calling external libraries
 * Copyright 2007-2011 The MathWorks, Inc.
 */

#include "HostLib_FFT.h"

#if defined(_WIN32)
const char *libName_FFT = "dspfft.dll";
#elif defined(__APPLE__)
const char *libName_FFT = "libmwdspfft.dylib";
#else
const char *libName_FFT = "libmwdspfft.so";
#endif

void LibCreate_FFT(void *hl,
                   const int FFTLength,
                   int nFFTs,
                   const void *inPtr,
                   void *outPtr,
                   int isIFFT,
                   int is2dOp,
                   int libraryControl,
                   unsigned int algType,
                   unsigned int floatType,
                   int canOverwriteInput,
                   int isCompact, 
                   int needBuffers,
                   int isInplace,
                   int nThread4Planner)
{
    HostLibrary *hostLib = (HostLibrary*)hl;
    *hostLib->errorMessage  = '\0';
    hostLib->instance = (MAKE_FCN_PTR(pFnLibCreate_FFT, hostLib->libCreate))(hostLib->errorMessage,
            FFTLength, nFFTs, inPtr, outPtr, isIFFT, is2dOp, libraryControl,
            algType, floatType, canOverwriteInput, isCompact, needBuffers, isInplace,
            nThread4Planner);
}

void LibOutputs_FFT(void *hl,
                    const void *inPtr,
                    void *outPtr,
                    int nRows,
                    int nCols)
{
    HostLibrary *hostLib = (HostLibrary*) hl;
    if(hostLib->instance) {
        *hostLib->errorMessage  = '\0';
        (MAKE_FCN_PTR(pFnLibOutputs_FFT,hostLib->libOutputs))(hostLib->instance,
            hostLib->errorMessage, inPtr, outPtr, nRows, nCols);
    }
}


