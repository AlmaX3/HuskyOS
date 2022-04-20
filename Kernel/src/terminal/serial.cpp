#include <hkStdio.h>
#include <serial.h>
#include <stdarg.h>

#define BUFSIZE 1024*8
static char buffer[BUFSIZE];
static int idx = 0;

int receivedSerial() {
    return inportb(PORT_COM1 + 5) & 1;
}
char readSerial() {
    while (receivedSerial() == 0)
        ;
    return inportb(PORT_COM1);
}

int isTransmitEmpty() {
    return inportb(PORT_COM1 + 5) & 0x20;
}

void serialPrint(const char Char) {
    while (isTransmitEmpty() == 0)
        ;
    outportb(PORT_COM1, Char);
}

void serialWrite(char *str) {
    while (*str)
        serialPrint(*(str++));
}

void flushSerial() {
    serialWrite(buffer);
    memset(buffer, 0, BUFSIZE);
    idx = 0;
}

void serialPutchar(const char Char) {
    if (idx + 1 > BUFSIZE)
        flushSerial();


    buffer[idx] = Char;
    buffer[idx+1] = '\0';
    idx++;


    if (Char == '\n') {
        flushSerial();
    }

}

void debug(const char *fmt, ...) {
    va_list(args);
    va_start(args, fmt);
    HuskyStandardOutput.vkprint(fmt, serialPutchar, args);
    va_end(args);
}
void initSerial() {
    memset(buffer, 0, BUFSIZE);
    outportb(PORT_COM1 + 1, 0x00); //interrupts off
	outportb(PORT_COM1 + 3, 0x80); //baud rate
	outportb(PORT_COM1 + 0, 0x03); //divisor to 3
	outportb(PORT_COM1 + 1, 0x00);
	outportb(PORT_COM1 + 3, 0x03); //1 byte, no parity, 1 stop bit
	outportb(PORT_COM1 + 2, 0xC7); //FIFO, 14-byte threshold
	outportb(PORT_COM1 + 4, 0x0B);
}
