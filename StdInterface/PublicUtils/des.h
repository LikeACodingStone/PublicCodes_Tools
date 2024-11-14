#ifndef __DES_H__
#define __DES_H__


//#define ENCR_DllExport

DLL_EXPORT void DES(unsigned char *puchCodeIn, unsigned char *puchCodeOut, unsigned char *puchKey);

DLL_EXPORT void UNDES(unsigned char *puchCodeIn, unsigned char *puchCodeOut, unsigned char *puchKey);

DLL_EXPORT bool triple_des(unsigned char *inputData, unsigned long ulInputDataLen, unsigned char *outputData, unsigned char *key);

DLL_EXPORT bool reversed_triple_des(unsigned char *inputData, unsigned long ulInputDataLen, unsigned char *outputData, unsigned char *key);

#endif //__DES_H__

