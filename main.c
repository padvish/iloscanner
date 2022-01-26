
#include "iLOAccess.h"
#include "sha256.h"
#include "whitelist.h"

void DisplayBanner()
{
    printf("Padvish iLO Scanner 1.0\n");
    printf("(C) Amnpardaz Software Company 2021\n\n");
}

void DisplayHelp()
{
    printf("Usage:\n");
    printf("\tiLOHardwareScanner <Command>\n");
    printf("\tCommands:\n");
    printf("\t\t-i:\tFind iLO in Hardware List\n");
    printf("\t\t-d:\tDump Firmware\n");
    return;
}
int main(
        const int    argc,
        const char** argv)
{
    setbuf(stdout, NULL);
    iLOInformation iliiLOInfo;
    BufferMemory   bmiLOFirmware;
    BufferMemory   bmFirmware;
    unsigned char  ucpath[64];
    
    if (argc == 1)
    {
        DisplayBanner();
        DisplayHelp();
        return 0;
    }
    if ((strcmp(argv[1],"-i") == 0) && argc == 2)
    {
        if(!GetiLOInfo())
        {
            printf("Could not get iLO info. (Did you run as root?)\n");
        }
        return 0;
    }
    if ((strcmp(argv[1],"-d") == 0) && argc == 2)
    {
        if (!DumpFirmware(&iliiLOInfo, &bmFirmware))
        {
            FreeBufferMemory(&bmFirmware);
            return 0;
        }

        sprintf(ucpath, "iLO_V%d_%u.bin", iliiLOInfo.itiLOVersion, (unsigned) time(NULL));
        if (!SaveFile(ucpath, bmFirmware))
        {
            printf("Could not save iLO Firmware.\n");
        }
        printf("File saved as %s.\n",ucpath);
        
        char digest[65];
        sha256_hex( bmFirmware.pcMemory, bmFirmware.ulSize, digest);
        printf("Firmware SHA-256: %s\n", digest);
        
        ilo_info *inf = FindILO(digest);
        if(inf) 
        {
        	printf("Congratulations, you've got a VALID firmware. You're NOT infected. (HP iLO %d - v%s)\n", inf->ilo, inf->version);
        }
        else 
        {
        	printf("\n");
        	printf("!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
        	printf("!!!! UNKNOWN FIRMWARE !!!!\n");
        	printf("!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
        	printf("\n");
        	printf("Your iLO firmware was not found in our whitelist database.\n");
        	printf("This may indicate the server has an infected firmware.\n");
        	printf("Please contact apt@amnpardaz.com and provide the '%s' file to analyse the file.\n", ucpath);
        }
        
        FreeBufferMemory(&bmFirmware);
        CloseAllAccess(&iliiLOInfo);
        return 0;
    }
    DisplayHelp();
    return 0;
}
