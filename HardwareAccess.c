
# include "HardwareAccess.h"

int   iDevMem;
void* pvMapping = NULL;
bool  MemoryPrepared = false;
off_t otMainMapBase = 0;

bool ReadPciRegister(
        PCIDeviceInformation pdiDevceInfo,
        const unsigned char ucPos,
        const unsigned char ucSize,
        void*      pucValue)
{
    unsigned char ucaPciDevicePath[512];
    BufferMemory bmConfigFile;

    sprintf(ucaPciDevicePath, "/sys/bus/pci/devices/%04x:%02x:%02x.%x/config",
            pdiDevceInfo.ucDom,
            pdiDevceInfo.ucBus,
            pdiDevceInfo.ucDev,
            pdiDevceInfo.ucFun);

    if (!LoadFile(ucaPciDevicePath, &bmConfigFile))
        return false;

    if (!memcpy(pucValue, bmConfigFile.pcMemory + ucPos, ucSize))
    {
        FreeBufferMemory(&bmConfigFile);
        return false;
    }

    FreeBufferMemory(&bmConfigFile);
    return true;
}

bool FindPciDevice(unsigned short usVid, unsigned short usDid, PPCIDeviceInformation ppdiDevceInfo)
{
    unsigned char ucaPciDevicePath[512];
    BufferMemory bmConfigFile;
    unsigned int uiNumberOfDirectory = 0;
    struct dirent *pdirAllPciDevice;
    DIR *pdDIR = opendir("/sys/bus/pci/devices/");
    if (pdDIR) {
        while (pdirAllPciDevice = readdir(pdDIR))
        {
            if (uiNumberOfDirectory < 2)
            {
                uiNumberOfDirectory++;
                continue;
            }
            sprintf(ucaPciDevicePath, "/sys/bus/pci/devices/%s/config",pdirAllPciDevice->d_name);
            if (!LoadFile(ucaPciDevicePath, &bmConfigFile))
                return false;
            unsigned short usDeviceVid;
            unsigned short usDeviceDid;
            memcpy(&usDeviceVid, bmConfigFile.pcMemory, sizeof(unsigned short));
            memcpy(&usDeviceDid, bmConfigFile.pcMemory + 2, sizeof(unsigned short));

            if (usDeviceVid == usVid && usDeviceDid == usDid)
            {
                ppdiDevceInfo->ucDom = atoi(pdirAllPciDevice->d_name + 0);
                ppdiDevceInfo->ucBus = atoi(pdirAllPciDevice->d_name + 5);
                ppdiDevceInfo->ucDev = atoi(pdirAllPciDevice->d_name + 8);
                ppdiDevceInfo->ucFun = atoi(pdirAllPciDevice->d_name + 11);
                memcpy(&ppdiDevceInfo->usVid, bmConfigFile.pcMemory + 0x00, sizeof(unsigned short));
                memcpy(&ppdiDevceInfo->usDid, bmConfigFile.pcMemory + 0x02, sizeof(unsigned short));
                memcpy(&ppdiDevceInfo->usSubVid, bmConfigFile.pcMemory + 0x2C, sizeof(unsigned short));
                memcpy(&ppdiDevceInfo->usSubDid, bmConfigFile.pcMemory + 0x2E, sizeof(unsigned short));
                FreeBufferMemory(&bmConfigFile);
                closedir(pdDIR);
                return true;
            }
            FreeBufferMemory(&bmConfigFile);
        }
    }
    closedir(pdDIR);
    return false;

}

bool PrepareMemory(off_t otMapBase)
{
    iDevMem = open("/dev/mem", O_RDWR);
    pvMapping = (unsigned  char*) mmap(NULL, 0xFF, PROT_READ | PROT_WRITE, MAP_SHARED, iDevMem, otMapBase);

    if (pvMapping == MAP_FAILED)
    {
        close(iDevMem);
        MemoryPrepared = false;
        return false;
    }
    otMainMapBase = otMapBase;
    MemoryPrepared = true;
    return true;
}

bool FreeHWMemory()
{
    if(MemoryPrepared)
    {
        close(iDevMem);
        pvMapping = NULL;
        MemoryPrepared = false;
        otMainMapBase = 0;
    }
    return true;
}

bool ReadMMIORegister(
        const unsigned long ulBaseAddress,
        const unsigned int  uiPos,
        const unsigned char ucSize,
        void*      pucValue)
{
    unsigned long ulMemoryAddress = ulBaseAddress + uiPos;

    long lPageSize = sysconf(_SC_PAGE_SIZE);

    off_t otMapBase = ulMemoryAddress & ~(lPageSize -1);
    off_t otExtraByte = ulMemoryAddress - otMapBase;
    if (!MemoryPrepared || (otMainMapBase != otMapBase))
    {
        if(!PrepareMemory(otMapBase))
            return false;
    }

    if (!memcpy(pucValue, (char *)pvMapping + otExtraByte, ucSize))
        return false;
    return true;
}


bool WriteMMIORegister(
        const unsigned long  ulBaseAddress,
        const unsigned int   uiPos,
        const unsigned char  ucSize,
        const void* pucValue)
{
    unsigned long ulMemoryAddress = ulBaseAddress + uiPos;

    long lPageSize = sysconf(_SC_PAGE_SIZE);

    off_t otMapBase = ulMemoryAddress & ~(lPageSize -1);
    off_t otExtraByte = ulMemoryAddress - otMapBase;
    if (!MemoryPrepared || (otMainMapBase != otMapBase))
    {
        if(!PrepareMemory(otMapBase))
            return false;
    }

    if (!memcpy((char *)pvMapping + otExtraByte, pucValue, ucSize))
        return false;
    return true;
}
