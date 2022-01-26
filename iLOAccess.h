
# include "HardwareAccess.h"

#define  HP_VID               0x103C
#define  iLO_DID              0x3307

#define  MAX_TRY_IOP_RESET    0x4
#define  MAX_DELAY_IOP_RESET  0x64

#define  FW_READ_CHUNK_SIZE   0x40

#define  SPI_CONTROLLER_INIT1  0x01
#define  SPI_CONTROLLER_INIT2  0x02
#define  SPI_CONTROLLER_CMD    0x05
#define  SPI_CONTROLLER_CHNKSZ 0x06
#define  SPI_CONTROLLER_OFFSET 0x08
#define  SPI_CONTROLLER_EXEC   0x04
#define  SPI_CONTROLLER_BUFFER 0x40



typedef enum iLOType{
    iLO_1 = 1,
    iLO_2 = 2,
    iLO_3 = 3,
    iLO_4 = 4,
    iLO_5 = 5
} iLOType;

typedef struct _iLOInformation{
    enum iLOType            itiLOVersion;
    PCIDeviceInformation    pdiPCIInfo;
    unsigned char           ucHWMemoryRegionOffset;
    unsigned int            uiHWMemoryRegionAddress;
    unsigned char           ucIOPControllerOffset;
    unsigned char           ucSPIMemoryRegionOffset;
    unsigned int            uiSPIMemoryRegionAddress;
    unsigned int            uiSPIAccessOffset;
    unsigned int            uiiLOControllerRegionOffset;
    unsigned long           uliLOChipSize;
}iLOInformation, *PiLOInformation;


bool GetiLOInfo();

bool DumpFirmware(
        PiLOInformation piiInformation,
        PBufferMemory pbmOutput);

bool CloseAllAccess(PiLOInformation piiInformation);
