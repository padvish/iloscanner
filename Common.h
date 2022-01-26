
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <memory.h>
#include <malloc.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <dirent.h>
#include <time.h>

typedef struct _BufferMemory{
    char* pcMemory;
    unsigned long  ulSize;
}BufferMemory, *PBufferMemory;

bool NewBufferMemory(
        PBufferMemory pbmNewMemory,
        unsigned long ulSize);

bool FreeBufferMemory(PBufferMemory pbmBufferMemory);

bool LoadFile(
        const unsigned char* pucPath,
        PBufferMemory        pbmOutput);

bool SaveFile(
        const unsigned char* pucPath,
        const BufferMemory   bmOutput);
