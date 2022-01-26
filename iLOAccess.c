
# include "iLOAccess.h"

bool bIsiLOInit = false;
bool bIsPrepareHWForDump = false;
const char* errorbuf = "";

bool _iLOInit(PiLOInformation piiInformation)
{
    if (!FindPciDevice(HP_VID, iLO_DID, &piiInformation->pdiPCIInfo))
    {
        errorbuf = "Could not find iLO device. (Did you run as root?)";
        return false;
    }

    switch (piiInformation->pdiPCIInfo.usSubDid)
    {
        case 0x3304:
        case 0x3305:
        {
            printf("Found iLO Version 2.\n");
            piiInformation->itiLOVersion = iLO_2;
            piiInformation->ucHWMemoryRegionOffset = 0x14;
            piiInformation->ucIOPControllerOffset = 0x40;
            piiInformation->ucSPIMemoryRegionOffset = 0x20;
            piiInformation->uiSPIAccessOffset = 0xAC;
            piiInformation->uiiLOControllerRegionOffset = 0x200;
            break;
        }
        case 0x3309:
        {
            printf("Found iLO Version 2 or 3.\n");
            piiInformation->itiLOVersion = iLO_2;
            piiInformation->ucHWMemoryRegionOffset = 0x14;
            piiInformation->ucIOPControllerOffset = 0x40;
            piiInformation->ucSPIMemoryRegionOffset = 0x20;
            piiInformation->uiSPIAccessOffset = 0xAC;
            piiInformation->uiiLOControllerRegionOffset = 0x200;
            break;
        }

        case 0x330E:
        {
            printf("Found iLO Version 3.\n");
            piiInformation->itiLOVersion = iLO_3;
            piiInformation->ucHWMemoryRegionOffset = 0x14;
            piiInformation->ucIOPControllerOffset = 0x40;
            piiInformation->ucSPIMemoryRegionOffset = 0x20;
            piiInformation->uiSPIAccessOffset = 0xAC;
            piiInformation->uiiLOControllerRegionOffset = 0x200;
            piiInformation->uliLOChipSize = 0x800000;
            break;
        }

        case 0x3381:
        {
            printf("Found iLO Version 4.\n");
            piiInformation->itiLOVersion = iLO_4;
            piiInformation->ucHWMemoryRegionOffset = 0x14;
            piiInformation->ucIOPControllerOffset = 0x40;
            piiInformation->ucSPIMemoryRegionOffset = 0x20;
            piiInformation->uiSPIAccessOffset = 0xAC;
            piiInformation->uiiLOControllerRegionOffset = 0x200;
            piiInformation->uliLOChipSize = 0x1000000;
            break;
        }

        case 0x00e4:
        {
            printf("Found iLO Version 5.\n");
            piiInformation->itiLOVersion = iLO_5;
            piiInformation->ucHWMemoryRegionOffset = 0x14;
            piiInformation->ucIOPControllerOffset = 0x40;
            piiInformation->ucSPIMemoryRegionOffset = 0x20;
            piiInformation->uiSPIAccessOffset = 0xAC;
            piiInformation->uiiLOControllerRegionOffset = 0x200;
            piiInformation->uliLOChipSize = 0x2000000;
            break;
        }

        default:
        {
            errorbuf = "Could not Find iLO Version";
            return false;
        }

    }
    bIsiLOInit = true;
    return true;
}

bool _PrepareHWForDump(PiLOInformation piiInformation)
{
    if (!bIsiLOInit)
    {
        if (!_iLOInit(piiInformation))
        {
            return false;
        }
    }
    if (!ReadPciRegister(
            piiInformation->pdiPCIInfo,
            piiInformation->ucHWMemoryRegionOffset,
            sizeof(unsigned int),
            &piiInformation->uiHWMemoryRegionAddress))
    {
        errorbuf = "Could not get hardware memory region address.";
        return false;
    }
    unsigned char ucTryCounter = 0;
    unsigned char ucIOPVal;
    unsigned char ucWRCMD;
    while (ucTryCounter <= MAX_TRY_IOP_RESET)
    {
        if(!ReadMMIORegister(piiInformation->uiHWMemoryRegionAddress, piiInformation->ucIOPControllerOffset, sizeof(unsigned char), &ucIOPVal))
            break;
        ucWRCMD = ucIOPVal & 0xF7;
        if(!WriteMMIORegister(piiInformation->uiHWMemoryRegionAddress, piiInformation->ucIOPControllerOffset, sizeof(unsigned char), &ucWRCMD))
            break;
        if(!ReadMMIORegister(piiInformation->uiHWMemoryRegionAddress, piiInformation->ucIOPControllerOffset, sizeof(unsigned char), &ucIOPVal))
            break;
        ucWRCMD = ucIOPVal | 0x08;
        if(!WriteMMIORegister(piiInformation->uiHWMemoryRegionAddress, piiInformation->ucIOPControllerOffset, sizeof(unsigned char), &ucWRCMD))
            break;
        unsigned char ucDelay = 0;
        do
        {
            usleep(200000);
            if(!ReadMMIORegister(piiInformation->uiHWMemoryRegionAddress, piiInformation->ucIOPControllerOffset, sizeof(unsigned char), &ucIOPVal))
                break;
            ucDelay++;

        }
        while (ucDelay < MAX_DELAY_IOP_RESET && !(ucIOPVal & 0x01));
        if ((ucIOPVal & 0x01))
            break;
        ucTryCounter++;
    }

    if (ucTryCounter > MAX_TRY_IOP_RESET)
    {
        errorbuf = "IOP isn't in reset!";
        return false;
    }

    unsigned int uiResetReadCMD;
    unsigned int uiResetWriteCMD;

    if(!ReadMMIORegister(piiInformation->uiHWMemoryRegionAddress, piiInformation->ucIOPControllerOffset, sizeof(unsigned int), &uiResetReadCMD))
    {
        errorbuf = "Could not reset IOP.";
        return false;
    }
    uiResetWriteCMD = uiResetReadCMD & 0xFFFFF7F6;
    if(!WriteMMIORegister(piiInformation->uiHWMemoryRegionAddress, piiInformation->ucIOPControllerOffset, sizeof(unsigned int), &uiResetWriteCMD))
    {
        errorbuf = "Could not reset IOP.";
        return false;
    }

    if(!ReadMMIORegister(piiInformation->uiHWMemoryRegionAddress, piiInformation->ucIOPControllerOffset, sizeof(unsigned int), &uiResetReadCMD))
    {
        errorbuf = "Could not reset IOP.";
        return false;
    }

    if(!ReadMMIORegister(piiInformation->uiHWMemoryRegionAddress,
                         piiInformation->ucSPIMemoryRegionOffset,
                         sizeof(unsigned int),
                         &piiInformation->uiSPIMemoryRegionAddress))
    {
        errorbuf = "Could not get SPI memory region address.";
        return false;
    }

    unsigned short  usCMD= 0x1000;
    if(!WriteMMIORegister(piiInformation->uiSPIMemoryRegionAddress,
                         piiInformation->uiSPIAccessOffset,
                         sizeof(unsigned short),
                          &usCMD))
    {
        errorbuf = "Could not set SPI access byte.";
        return false;
    }

    bIsPrepareHWForDump = true;
    return true;
}

bool _WaitForResponse(PiLOInformation piiInformation)
{
    unsigned ucRepValue;
    do
    {
        if(!ReadMMIORegister(piiInformation->uiSPIMemoryRegionAddress,
                              piiInformation->uiiLOControllerRegionOffset + SPI_CONTROLLER_EXEC,
                              sizeof(unsigned char),
                              &ucRepValue))
        {
            errorbuf = "Could not write dump command.";
            return false;
        }
        if (!ucRepValue)
            break;
    }
    while(false);
    return true;
};

bool _FlashCleanUp(PiLOInformation piiInformation)
{
    if (!bIsPrepareHWForDump)
    {
        errorbuf = "Not ready for IOP reset.";
        return false;
    }

    unsigned short  usCMD= 0x0000;
    if(!WriteMMIORegister(piiInformation->uiSPIMemoryRegionAddress,
                          piiInformation->uiSPIAccessOffset,
                          sizeof(unsigned short),
                          &usCMD))
    {
        errorbuf = "Could not set SPI access byte.";
        return false;
    }

    unsigned int uiIOPVal;

    if(!ReadMMIORegister(piiInformation->uiHWMemoryRegionAddress, piiInformation->ucIOPControllerOffset, sizeof(unsigned int), &uiIOPVal))
    {
        errorbuf = "Could not get IOP.";
        return false;
    }

    usleep(1000);
    unsigned char uiSignalVal = uiIOPVal & 0xFFFFFFF7 | 0x01;

    if(!WriteMMIORegister(piiInformation->uiHWMemoryRegionAddress, piiInformation->ucIOPControllerOffset, sizeof(unsigned char), &uiSignalVal))
    {
        errorbuf = "Could not reset IOP.";
        return false;
    }

    return true;
}

bool _DumpiLOFirmware(PiLOInformation piiInformation, PBufferMemory pbmOutput)
{
    unsigned char ucChunkSize = FW_READ_CHUNK_SIZE;
    NewBufferMemory(pbmOutput, piiInformation->uliLOChipSize);
    unsigned int uiOffset;
    unsigned char ucCMD;
    printf("Dumping iLO Firmware. Please Wait...\n");
    for (uiOffset = 0; uiOffset < piiInformation->uliLOChipSize; uiOffset+= ucChunkSize)
    {
        ucCMD = 0x0B;
        if(!WriteMMIORegister(piiInformation->uiSPIMemoryRegionAddress,
                              piiInformation->uiiLOControllerRegionOffset + SPI_CONTROLLER_INIT1,
                              sizeof(unsigned char),
                              &ucCMD))
        {
            printf("Could not write dump command.\n");
            return false;
        }

        ucCMD = 0x03;
        if(!WriteMMIORegister(piiInformation->uiSPIMemoryRegionAddress,
                              piiInformation->uiiLOControllerRegionOffset + SPI_CONTROLLER_INIT2,
                              sizeof(unsigned char),
                              &ucCMD))
        {
            printf("Could not write dump command.\n");
            return false;
        }

        ucCMD = 0x0B;
        if(!WriteMMIORegister(piiInformation->uiSPIMemoryRegionAddress,
                              piiInformation->uiiLOControllerRegionOffset + SPI_CONTROLLER_CMD,
                              sizeof(unsigned char),
                              &ucCMD))
        {
            printf("Could not write dump command.\n");
            return false;
        }

        if(!WriteMMIORegister(piiInformation->uiSPIMemoryRegionAddress,
                              piiInformation->uiiLOControllerRegionOffset + SPI_CONTROLLER_CHNKSZ,
                              sizeof(unsigned char),
                              &ucChunkSize))
        {
            printf("Could not write dump command.\n");
            return false;
        }

        if(!WriteMMIORegister(piiInformation->uiSPIMemoryRegionAddress,
                              piiInformation->uiiLOControllerRegionOffset + SPI_CONTROLLER_OFFSET,
                              sizeof(unsigned int),
                              &uiOffset))
        {
            printf("Could not write dump command.\n");
            return false;
        }



        ucCMD = 0x05;
        if(!WriteMMIORegister(piiInformation->uiSPIMemoryRegionAddress,
                              piiInformation->uiiLOControllerRegionOffset + SPI_CONTROLLER_EXEC,
                              sizeof(unsigned char),
                              &ucCMD))
        {
            printf("Could not write dump command.\n");
            return false;
        }

        if (!_WaitForResponse(piiInformation))
        {
            printf("iLO HW not response.\n");
            return false;
        }
        usleep(100);
        if(!ReadMMIORegister(piiInformation->uiSPIMemoryRegionAddress,
                              piiInformation->uiiLOControllerRegionOffset + SPI_CONTROLLER_BUFFER,
                              ucChunkSize,
                              pbmOutput->pcMemory + uiOffset))
        {
            printf("Could not write dump command.\n");
            return false;
        }
        printf("Read 0x%x byte from 0x%lx\r", uiOffset, piiInformation->uliLOChipSize);


    }
    printf("iLO firmware dump completed.\n");
    return true;
}

bool GetiLOInfo()
{
    iLOInformation iliiLOInfo;
    return _iLOInit(&iliiLOInfo);
}

bool DumpFirmware(PiLOInformation piiInformation, PBufferMemory pbmOutput)
{
    bool bResult = false;
    if (!bIsPrepareHWForDump)
    {
        if (!_PrepareHWForDump(piiInformation))
        {
            fprintf(stderr, "%s\n", errorbuf);
            fprintf(stderr, "Failed to access iLO device, Please check iLO Security Switch.\n");
            return false;
        }
    }
    bResult =_DumpiLOFirmware(piiInformation, pbmOutput);

    return bResult;
}

bool CloseAllAccess(PiLOInformation piiInformation)
{
    if (!_FlashCleanUp(piiInformation))
    {
        fprintf(stderr, "%s\n", errorbuf);
        return false;
    }
    FreeHWMemory();
    return true;
}
