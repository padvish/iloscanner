
# include "Common.h"

bool NewBufferMemory(
        PBufferMemory pbmNewMemory,
        unsigned long ulSize)
{
    if(ulSize < 0)
        return false;

    pbmNewMemory->pcMemory = (char *) malloc(ulSize);
    if (!pbmNewMemory->pcMemory)
        return false;
    pbmNewMemory->ulSize = ulSize;
    memset(pbmNewMemory->pcMemory, 0, ulSize);

    return true;
}

bool FreeBufferMemory(PBufferMemory pbmBufferMemory)
{
    free(pbmBufferMemory->pcMemory);
    pbmBufferMemory->ulSize = 0;
}

bool LoadFile(
        const unsigned char* pucPath,
        PBufferMemory        pbmOutput)
{
    if (!(pucPath && pbmOutput))
        return false;

    FILE* pfFD = fopen(pucPath, "rb");
    if (!pfFD)
        return false;

    fseek(pfFD,0, SEEK_END);
    unsigned long ulSize = ftell(pfFD);
    fseek(pfFD,0, SEEK_SET);
    if (ulSize < 1)
    {
        fclose(pfFD);
        return false;
    }

    if (!NewBufferMemory(pbmOutput,ulSize))
    {
        fclose(pfFD);
        return false;
    }

    int iReadSize = fread(pbmOutput->pcMemory, 1, ulSize, pfFD);

    if (iReadSize != ulSize)
    {
        fclose(pfFD);
        FreeBufferMemory(pbmOutput);
        return false;
    }

    fclose(pfFD);
    return true;
}

bool SaveFile(
        const unsigned char* pucPath,
        const BufferMemory   bmOutput)
{
    if (!(pucPath && bmOutput.pcMemory))
        return false;

    FILE* pfFD = fopen(pucPath, "wb");
    if (!pfFD)
        return false;

    if (bmOutput.ulSize < 1)
    {
        fclose(pfFD);
        return false;
    }

    int iWriteSize = fwrite(bmOutput.pcMemory, 1, bmOutput.ulSize, pfFD);

    if (iWriteSize != bmOutput.ulSize)
    {
        fclose(pfFD);
        return false;
    }

    fclose(pfFD);
    return true;
}