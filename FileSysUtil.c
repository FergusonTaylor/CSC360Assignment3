#include "FileSysUtil.h"

void GetFileName(char* fileName, char* rootEntry)
{
    char fileExtension[4];
    int i = 0;
    while( i < 8 && rootEntry[i] != ' ' )
    {
        fileName[i] = rootEntry[i];
        i++;
    }
    fileName[i] = '.';
    fileName[i+1] = '\0';
    int j = 0;
    while( j < 3 && rootEntry[j+8] != ' ' )
    {
        fileExtension[j] = rootEntry[j+8];
        j++;
    }
    strcat(fileName, fileExtension);
    fileName[i+j+1] = '\0';
    return;
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
int GetFileSize( char* rootEntry)
{
    int fileSize;
    unsigned char fileSizeBytes[4];
    int i;
    for(i = 0 ; i < 4; i++)
    {
        fileSizeBytes[i] = rootEntry[28 +i];
    }
    fileSize = LittleEndianBytesToInt(fileSizeBytes,4);
    return fileSize;
}