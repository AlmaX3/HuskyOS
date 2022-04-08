#pragma once
#include <stddef.h>
#include <stdint.h>

void outportb(unsigned short _port, unsigned char _data);
unsigned char inportb(unsigned short _port);
uint16_t inportw(uint16_t _port);
void outportw(uint16_t _port, uint16_t _data);
uint32_t inportl(uint16_t _port);
void outportl(uint16_t _port, uint32_t _data);
void outportsm(unsigned short port, unsigned char *data, unsigned long size);
void inportsm(unsigned short port, unsigned char *data, unsigned long size);
void io_wait();

size_t strlen(const char *str);
void strcpy(const char *src, char *dst);
int strcmp(const char *p, const char *q);
size_t rfind(const char *str, const char accept);
size_t lfind(const char *str, const char accept);
int strncmp(char *str1, char *str2, int len);
size_t strspn(const char *str, const char *accept);
char *strpbrk(const char *str, const char *accept);
char *strtok_r(char *str, const char *delim, char **saveptr);

void *memmove(void *dstptr, const void *srcptr, size_t size);
int memcmp(const void *aptr, const void *bptr, size_t size);
void memcpy(void *dstptr, const void *srcptr, size_t size);
void *memset(void *pointer, uint32_t value, size_t size);
