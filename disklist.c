#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char GetFileType(char* rootEntry)
{
    
    char attr = rootEntry[11];
    //checks if entry is teh volume label
    if((attr & 0x08) == 0x08 && (attr & 0xF5) == 0)
    {
        return 'N';
    }
    if(attr != 0x0F && (attr & 0x08) == 0 && rootEntry[0] != 0xE5 && rootEntry[0] != 0)
    {
        return 'F';
    }
    if((attr & 0x10) != 0)
    {
        return 'D';
    }
    return 'N';
}
void GetFileName(char* fileName,char* rootEntry)
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
int main(int argc, char *argv[])
{

    char fileType;
    char fileSize[11];
    char fileName[21];
    char creationDate[32];
    char creationTime[32];

    char rootEntry[32];

    FILE *fileptr;

    fileptr = fopen(argv[1], "r");
    if(fileptr == NULL)
    {
        printf("failed to open file \n");
        return -1;
    }

    //put fileptr at start of root directory
    fseek(fileptr, 512*19,SEEK_SET);
    int i = 0;
    while (i < 224)
    {
        fread(rootEntry,1 ,32,fileptr);
        fileType = GetFileType(rootEntry);
        if(fileType == 'N')
        {
            i++;
            continue;
        }
        GetFileName(fileName, rootEntry); 
        i++;
        printf("%c %10s %20s %s %s\n", fileType, fileSize, fileName, creationDate,creationTime);
    }
    fclose(fileptr);
    
}