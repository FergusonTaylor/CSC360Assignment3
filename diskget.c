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
int GetInfoFromRoot(char* fileName,int* clusterNumer,int* fileSize, FILE* fileptr)
{
    char rootEntry[32];
    char rootFileName[16];
    //put fileptr to root
    fseek(fileptr, 512*19,SEEK_SET);
    int i = 0;
    while (i < 224)
    {
        fread(rootEntry,1 ,32,fileptr);
        GetFileName(rootFileName, rootEntry);
        //if(strcomp(fileName))
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
    int clusterNumer;
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
    if (GetInfoFromRoot(fileName, &clusterNumer,&fileSize,fileptr) == -1)
    {
        printf("could not find file %s\n",fileName);
    }
    fclose(fileptr);
}