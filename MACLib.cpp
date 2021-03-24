//#define LOG_NDEBUG 0
#define LOG_TAG "MACLib"
#include "utils/Log.h"

#include "All.h"
#include "MACLib.h"

#if 0
#include "APECompress.h"
#include "APECompressCreate.h"
#include "APECompressCore.h"
#endif
#include "APEDecompress.h"
#include "APEInfo.h"
#include "APELink.h"

#ifdef BACKWARDS_COMPATIBILITY
    #include "Old/APEDecompressOld.h"
#endif

IAPEDecompress * CreateIAPEDecompressCore(CAPEInfo * pAPEInfo, int nStartBlock, int nFinishBlock, int * pErrorCode)
{
    // create the decompressor (this eats the CAPEInfo object)
    IAPEDecompress * pAPEDecompress = NULL;
    CAPEDecompress * pCAPEDecompress = NULL;

    // proceed if we have an info object
    if (pAPEInfo != NULL)
    {
        // proceed if there's no error with the info object
        if (*pErrorCode == ERROR_SUCCESS)
        {
//            try
            {
                // create
                if (pAPEInfo->GetInfo(APE_INFO_FILE_VERSION) >= 3930)
                {
                      pAPEDecompress = new CAPEDecompress(pErrorCode, pAPEInfo, nStartBlock, nFinishBlock);
		    //
                    //pAPEDecompress = pCAPEDecompress;
                }
#ifdef BACKWARDS_COMPATIBILITY
                else
                {
                    pAPEDecompress = new CAPEDecompressOld(pErrorCode, pAPEInfo, nStartBlock, nFinishBlock);
                }
#endif

                // error check
                if (pAPEDecompress == NULL || *pErrorCode != ERROR_SUCCESS)
                {
                    SAFE_DELETE(pAPEDecompress)
                }
            }
#if 0
            catch(...)
            {
                SAFE_DELETE(pAPEDecompress)
                *pErrorCode = ERROR_UNDEFINED;
            }
#endif
        }
        else
        {
            // eat the CAPEInfo object if we didn't create a decompressor
            SAFE_DELETE(pAPEInfo)
        }
    }

    // return
    return pAPEDecompress;
}

IAPEDecompress * __stdcall CreateIAPEDecompress(const char * pFilename, int * pErrorCode)
{
    // error check the parameters
    //ALOGV("pFilename = %s", pFilename);
    if ((pFilename == NULL) || (strlen(pFilename) == 0))
    {
        if (pErrorCode) *pErrorCode = ERROR_BAD_PARAMETER;
        ALOGV("check ape parameters error!!!\n");
        return NULL;
    }

    // variables
    int nErrorCode = ERROR_UNDEFINED;
    CAPEInfo * pAPEInfo = NULL;
    int nStartBlock = -1;
    int nFinishBlock = -1;

    // get the extension
    const char  * pExtension = &pFilename[strlen(pFilename)];
    while ((pExtension > pFilename) && (*pExtension != '.'))
    {
        pExtension--;
    }

    // take the appropriate action (based on the extension)
    if (strcasecmp(pExtension, ".apl") == 0)
    {
        // "link" file (.apl linked large APE file)
        CAPELink APELink(pFilename);
        if (APELink.GetIsLinkFile())
        {
            pAPEInfo = new CAPEInfo(&nErrorCode, APELink.GetImageFilename(), new CAPETag(pFilename, TRUE));
            nStartBlock = APELink.GetStartBlock();
            nFinishBlock = APELink.GetFinishBlock();
        }
    }
    /*Oplus 2011-02-12 zhouhy Modify for not judge file extension*/
    else
    //else if ((strcasecmp(pExtension, ".mac") == 0) || (strcasecmp(pExtension, ".ape") == 0))
    {
        // plain .ape file
        pAPEInfo = new CAPEInfo(&nErrorCode, pFilename);
    }

    // fail if we couldn't get the file information
    if (pAPEInfo == NULL)
    {
        if (pErrorCode)
        {
            *pErrorCode = ERROR_INVALID_INPUT_FILE;
        }
        ALOGV("Can not get the file information");
        return NULL;
    }

    // create
    IAPEDecompress * pAPEDecompress = CreateIAPEDecompressCore(pAPEInfo, nStartBlock, nFinishBlock, &nErrorCode);
    if (pErrorCode)
    {
        //ALOGE("CreateIAPEDecompressCore_nErrorCode = %d", nErrorCode);
        *pErrorCode = nErrorCode;
    }

    // return
    return pAPEDecompress;
}

IAPEDecompress * __stdcall CreateIAPEDecompressEx(CIO * pIO, int * pErrorCode)
{
    // create info
    int nErrorCode = ERROR_UNDEFINED;
    CAPEInfo * pAPEInfo = new CAPEInfo(&nErrorCode, pIO);

    // create decompress core
    IAPEDecompress * pAPEDecompress = CreateIAPEDecompressCore(pAPEInfo, -1, -1, &nErrorCode);
    if (pErrorCode)
    {
        *pErrorCode = nErrorCode;
    }

    // return
    return pAPEDecompress;
}

IAPEDecompress * __stdcall CreateIAPEDecompressEx2(CAPEInfo * pAPEInfo, int nStartBlock, int nFinishBlock, int * pErrorCode)
{
    int nErrorCode = ERROR_SUCCESS;
    IAPEDecompress * pAPEDecompress = CreateIAPEDecompressCore(pAPEInfo, nStartBlock, nFinishBlock, &nErrorCode);
    if (pErrorCode)
    {
        *pErrorCode = nErrorCode;
    }
    return pAPEDecompress;
}
#if 0
IAPECompress * __stdcall CreateIAPECompress(int * pErrorCode)
{
    if (pErrorCode)
    {
        *pErrorCode = ERROR_SUCCESS;
    }

    return new CAPECompress();
}
#endif
int __stdcall FillWaveFormatEx(WAVEFORMATEX * pWaveFormatEx, int nSampleRate, int nBitsPerSample, int nChannels)
{
    pWaveFormatEx->cbSize = 0;
    pWaveFormatEx->nSamplesPerSec = nSampleRate;
    pWaveFormatEx->wBitsPerSample = nBitsPerSample;
    pWaveFormatEx->nChannels = nChannels;
    pWaveFormatEx->wFormatTag = 1;

    pWaveFormatEx->nBlockAlign = (pWaveFormatEx->wBitsPerSample / 8) * pWaveFormatEx->nChannels;
    pWaveFormatEx->nAvgBytesPerSec = pWaveFormatEx->nBlockAlign * pWaveFormatEx->nSamplesPerSec;

    return ERROR_SUCCESS;
}

int __stdcall FillWaveHeader(WAVE_HEADER * pWAVHeader, int nAudioBytes, WAVEFORMATEX * pWaveFormatEx, int nTerminatingBytes)
{
//    try
    {
        // RIFF header
        memcpy(pWAVHeader->cRIFFHeader, "RIFF", 4);
        pWAVHeader->nRIFFBytes = (nAudioBytes + 44) - 8 + nTerminatingBytes;

        // format header
        memcpy(pWAVHeader->cDataTypeID, "WAVE", 4);
        memcpy(pWAVHeader->cFormatHeader, "fmt ", 4);

        // the format chunk is the first 16 bytes of a waveformatex
        pWAVHeader->nFormatBytes = 16;
        memcpy(&pWAVHeader->nFormatTag, pWaveFormatEx, 16);

        // the data header
        memcpy(pWAVHeader->cDataHeader, "data", 4);
        pWAVHeader->nDataBytes = nAudioBytes;

        return ERROR_SUCCESS;
    }
#if 0
    catch(...)
    {
        return ERROR_UNDEFINED;
    }
#endif
}