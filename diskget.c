#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "FileSysUtil.h"
void ToUpper(char* string)
{
    int i = 0;
    while(string[i] != '\0')
    {
        string[i] = toupper((unsigned char)string[i]);
        i++;
    }
    return;
}

int GetInfoFromRoot(char* fileName,int* clusterNumber,int* fileSize, FILE* fileptr)
{
    char rootEntry[32];
    char rootFileName[16];
    char clusterNumberBytes[2];
    //put fileptr to root
    fseek(fileptr, 512*19,SEEK_SET);
    int i = 0;
    while (i < 224)
    {
        fread(rootEntry,1 ,32,fileptr);
        GetFileName(rootFileName, rootEntry);
        if(!strcmp(fileName, rootFileName))
        {
            clusterNumberBytes[0] = rootEntry[26];
            clusterNumberBytes[1] = rootEntry[27];
            *clusterNumber = LittleEndianBytesToInt(clusterNumberBytes,2) +31;
            *fileSize = GetFileSize(rootEntry);
            return 1;
        }
        i++;
    }
    return  -1;
}

void GetClusterInformation(int clusterNumber, char* buffer, FILE* fileptr)
{
    fseek(fileptr,clusterNumber*512,SEEK_SET);
    fread(buffer, 1, 512,fileptr);
}
int FindNextCluster(FILE* fileptr, int previousCluster)
{
    int FATIndex = previousCluster - 31;
    unsigned char twoFatEntries[3];
    unsigned char fatEntry[2];
    //get to start of FAT Table
    fseek(fileptr, 512+(3*(FATIndex/2)) ,SEEK_SET);
    fread(twoFatEntries,1,3,fileptr);
    if(FATIndex % 2)
    {
        fatEntry[0] = (twoFatEntries[1] >> 4) | (twoFatEntries[2] << 4);
        fatEntry[1] = (twoFatEntries[2] >> 4);
    }
    else
    {
        fatEntry[0] = twoFatEntries[0];
        fatEntry[1] = (twoFatEntries[1] & 0x0F);
    }
    int nextCluster = LittleEndianBytesToInt(fatEntry,2)+31;
    return nextCluster;
}
int main(int argc, char *argv[])
{
    if(argc != 3)
    {
        printf("we want 3 arguments not: %d arguments", argc);
        return -1;
    }
    char fileName[16];
    char upperFileName[16];
    char buffer[512];
    int clusterNumber;
    int fileSize;

    strcpy(fileName, argv[2]); 
    strcpy(upperFileName, argv[2]); 
    ToUpper(upperFileName);

    FILE* fileptr;

    fileptr = fopen(argv[1], "r");
    if(fileptr == NULL)
    {
        printf("failed to open file \n");
        return -1;
    }
    if (GetInfoFromRoot(upperFileName, &clusterNumber,&fileSize,fileptr) == -1)
    {
        printf("File not found\n");
    }
    FILE* toWriteptr = fopen(fileName, "w");
    
    while(fileSize !=0)
    {
        GetClusterInformation(clusterNumber,buffer,fileptr);
        
        if(fileSize < 512)
        {
            fwrite(buffer, 1, fileSize, toWriteptr);
            fileSize = 0;
        }
        else
        {
            fwrite(buffer, 1, 512, toWriteptr);
            clusterNumber = FindNextCluster(fileptr, clusterNumber); 
            fileSize = fileSize - 512;
        }
    }
    fclose(toWriteptr);
    fclose(fileptr);
}