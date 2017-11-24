#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
            *clusterNumber = LittleEndianBytesToInt(clusterNumberBytes,2);
            *fileSize = GetFileSize(rootEntry);
            return 1;
        }
        i++;
    }
    return  -1;
}
int main(int argc, char *argv[])
{
    if(argc != 3)
    {
        printf("we want 3 arguments not: %d arguments", argc);
        return;
    }
    char fileName[16];
    int clusterNumber;
    int fileSize;
    strcpy(fileName, argv[2]); 
    ToUpper(fileName);
    printf("%s\n",fileName);

    FILE* fileptr;

    fileptr = fopen(argv[1], "r");
    if(fileptr == NULL)
    {
        printf("failed to open file \n");
        return -1;
    }
    if (GetInfoFromRoot(fileName, &clusterNumber,&fileSize,fileptr) == -1)
    {
        printf("File not found\n");
    }
    while(fileSize !=0)
    {
        if(fileSize < 512)
        {

            fileSize = 0;
        }
        
        fileSize - 512;
    }
    fclose(fileptr);
}