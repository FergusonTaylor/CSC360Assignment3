#include <stdio.h>
#include <stdlib.h>
#include <string.h>
void ReadXBytes(int offset,int numberToRead, FILE* fileptr, char* readInto)
{
    fseek(fileptr, offset, SEEK_SET);
    fread(readInto,1 ,numberToRead,fileptr);
}
unsigned int LittleEndianBytesToInt(unsigned char* bytes, int length)
{
    unsigned int powerOf2 = 1;
    unsigned int finalInt = 0;
    int i;
    for( i = 0; i < length; i++)
    {
        finalInt += powerOf2*bytes[i];
        powerOf2 = powerOf2*256;
    }
    return finalInt;
}
int CountFreeSectors(FILE* fileptr, int numberOfSectors)
{
    char twoFATEntry[3];
    int sectorCount = 0;
    //put fileptr at start of FAT1
    fseek(fileptr, 514,SEEK_SET);
    int i;
    i = 32;
    while (i < numberOfSectors)
    {
        fread(twoFATEntry,1 ,3,fileptr);
        i++;
        if(twoFATEntry[0] == 0 && (twoFATEntry[1] & 0x0F) == 0)
        {
            sectorCount += 1;
        }
        i++;
        
        if(i < numberOfSectors && (twoFATEntry[1] & 0xF0) == 0 && twoFATEntry[2] == 0)
        {
            sectorCount += 1;
        }
    }
    return sectorCount;
}
int CountRootFiles(FILE* fileptr)
{
    char rootEntry[32];
    char attr;
    int fileCount = 0;

    //set up file ptr
    fseek(fileptr, 512*19,SEEK_SET);
    int i = 0;
    while (i < 224)
    {
        fread(rootEntry,1 ,32,fileptr);
        attr = rootEntry[11];
        if(attr != 0x0F && (attr & 0x08) == 0 && rootEntry[0] != 0xE5 && rootEntry[0] != 0)
        {
            fileCount++;
        }
        i++;
    }

    return fileCount;
}
void GetVolumeLabelFromRoot(FILE *fileptr, char *diskLabel)
{
    char rootEntry[32];
    char attr;
    //set up file ptr
    fseek(fileptr, 512*19,SEEK_SET);
    int i = 0;
    while (i < 224)
    {
        fread(rootEntry,1 ,32,fileptr);

        attr = rootEntry[11];
        if((attr & 0x08) == 0x08 && (attr & 0xF5) == 0)
        {
            strncpy(diskLabel, rootEntry, 8);
            diskLabel[8] = '\0';
            break;
        }
        i++;
    }
    return;
}
int ComputeSizeInBytes(char bytesPerSector[2],char totalSectorCount[2])
{
    int totalSize = 0;
    int bytesPer = LittleEndianBytesToInt(bytesPerSector,2);
    int sectorCount = LittleEndianBytesToInt(totalSectorCount,2);
    return bytesPer * sectorCount;
}

int main(int argc, char *argv[])
{
    char OSName[11];
    char diskLabel[11];
    char freeSize;

    char bytesPerSector[2];
    char totalSectorCount[2];
    char numFATCopies[1];
    char sectorsPerFAT[2];
    int freeSectors;
    int numberOfFileInRoot;
    int totalSize = 0;
    FILE *fileptr;

    fileptr = fopen(argv[1], "r");
    if(fileptr == NULL)
    {
        printf("failed to open file \n");
        return -1;
    }
    ReadXBytes(3,8,fileptr,OSName);
    

    ReadXBytes(11,2,fileptr,bytesPerSector);
    ReadXBytes(19,2,fileptr,totalSectorCount);
    totalSize = ComputeSizeInBytes(bytesPerSector,totalSectorCount);
   
    ReadXBytes(16,1,fileptr,numFATCopies);
    ReadXBytes(22,2,fileptr,sectorsPerFAT);
    freeSectors = CountFreeSectors(fileptr, LittleEndianBytesToInt(totalSectorCount,2));
    numberOfFileInRoot = CountRootFiles(fileptr);
    GetVolumeLabelFromRoot(fileptr, diskLabel);

    printf("OSN name: %s\n", OSName);
    printf("Label of the disk: %s\n", diskLabel);
    printf("Total size of the disk: %d bytes\n", totalSize);
    printf("Free size of the disk: %d bytes\n", 512*freeSectors);

    printf("\n==============\n");
    printf("The number of files in the root directory (not including subdirectories): %d\n",numberOfFileInRoot);
    printf("\n==============\n");
    printf("Number of FATS: %d\n", numFATCopies[0]);
    printf("Sectors per FAT: %d\n", LittleEndianBytesToInt(sectorsPerFAT,2));

    fclose(fileptr);
}