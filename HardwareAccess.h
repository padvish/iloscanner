
# include "Common.h"

typedef struct _PCIDeviceInformation
{
    unsigned char   ucDom;
    unsigned char   ucBus;
    unsigned char   ucDev;
    unsigned char   ucFun;
    unsigned short  usVid;
    unsigned short  usDid;
    unsigned short  usSubVid;
    unsigned short  usSubDid;
}PCIDeviceInformation, *PPCIDeviceInformation;

bool ReadPciRegister(
        PCIDeviceInformation pdiDevceInfo,
        const unsigned char  ucPos,
        const unsigned char  ucSize,
        void*       pucValue);

bool FindPciDevice(
        unsigned short usVid,
        unsigned short usDid,
        PPCIDeviceInformation ppdiDevceInfo);

bool FreeHWMemory();

bool ReadMMIORegister(
        const unsigned long ulBaseAddress,
        const unsigned int  uiPos,
        const unsigned char ucSize,
        void*      pucValue);

bool WriteMMIORegister(
        const unsigned long  ulBaseAddress,
        const unsigned int   uiPos,
        const unsigned char  ucSize,
        const void* pucValue);
