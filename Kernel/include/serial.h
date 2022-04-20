#pragma once
#include "string.h"
#define PORT_COM1 0x3f8

int receivedSerial();
char readSerial();
int isTransmitEmpty();
void flushSerial();
void serialPrint(const char Char);
void serialPutchar(const char Char);
void debug(const char* fmt, ...);
void initSerial();

