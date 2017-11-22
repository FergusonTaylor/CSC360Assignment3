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
void CopyBytes(char* dest,char* source, int size, int offset)
{
    int i;
    for(i = 0; i < size; i++)
    {
        dest[i] = source[i + offset];
    }
}
void GetCreateDate(char* creationDate,char* rootEntry)
{
    int year;
    int month;
    int day;

    unsigned char yearBytes[1];
    unsigned char monthBytes[2];
    unsigned char dayBytes[1];
    CopyBytes(yearBytes, rootEntry, 1, 17);
    CopyBytes(monthBytes, rootEntry, 2, 16); 
    CopyBytes(dayBytes, rootEntry, 1, 16); 

    year = (yearBytes[0] >> 1) +1980;
    month = (monthBytes[1] & 0x01)*8 + (monthBytes[0] >> 5);
    day = (dayBytes[0] & 0x1F);

    char strYear[8];
    char strMonth[8];
    char strDay[8];

    sprintf(strYear, "%d", year);
    sprintf(strMonth, "%02d", month);
    sprintf(strDay, "%02d", day);

    creationDate[0] = '\0';
    strcat(creationDate,strYear);
    strcat(creationDate,"-");
    strcat(creationDate,strMonth);
    strcat(creationDate,"-");
    strcat(creationDate,strDay);
}
void GetCreateTime(char* creationTime,char* rootEntry)
{
    int hour;
    int min;

    unsigned char hourBytes[1];
    unsigned char minBytes[2];
    CopyBytes(hourBytes, rootEntry, 1, 15);
    CopyBytes(minBytes, rootEntry, 2, 14);

    hour = (hourBytes[0] >> 3);
    min = (minBytes[0] >>5 ) + (minBytes[1] & 0x07)*8;
    char strHour[8];
    char strMin[8];

    sprintf(strHour, "%02d", hour);
    sprintf(strMin, "%02d", min);

    creationTime[0] = '\0';
    strcat(creationTime,strHour);
    strcat(creationTime,":");
    strcat(creationTime,strMin);
    return;
}
int main(int argc, char *argv[])
{

    char fileType;
    int fileSize;
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
        fileSize = GetFileSize(rootEntry);
        GetFileName(fileName, rootEntry);
        GetCreateDate(creationDate, rootEntry);
        GetCreateTime(creationTime, rootEntry);
        i++;
        printf("%c %10d %20s %s %s\n", fileType, fileSize, fileName, creationDate,creationTime);
    }
    fclose(fileptr);
    
}