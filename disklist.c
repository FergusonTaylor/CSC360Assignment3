#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char GetFileType(char* rootEntry)
{
    
}
int main(int argc, char *argv[])
{

    char fileType;
    char fileSize[11];
    char fileName[21];
    char creationDate[32];
    char creationTime[32];

    char rootEntry[32];

    printf("%c %10s %20s %s %s", fileType, fileSize, fileName, creationDate,creationTime);

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
    }
    fclose(fileptr);
}