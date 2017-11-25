#ifndef FILESYSUTIL_H   /* Include guard */
#define FILESYSUTIL_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int GetFileSize( char* rootEntry);
unsigned int LittleEndianBytesToInt(unsigned char* bytes, int length);
void GetFileName(char* fileName, char* rootEntry);
#endif // FOO_H_