#include "../../include/string.h"

size_t strlen(const char* str) {
    size_t len = 0;
    while (str[len])
        len++;
    return len;
    
}

void strcpy(const char* src, char* dst) {
    size_t len = strlen(src);
    for (size_t i = 0; i < len ; i++)
    {
        dst[i] = src[i];
    }
}


int strcmp(const char* p, const char* q){
    while (*p || *q) {
		if (*p != *q){
			return -1;
        }
		p++, q++;
	}
	return 0;
}


void* memmove(void* dstptr, const void* srcptr, size_t size) {
    unsigned char* dst = (unsigned char*) dstptr;
    const unsigned char* src = (const unsigned char*) srcptr;
    if(dst < src){
        for (size_t i = 0; i < size; i++)
        {
            dst[i] = src[i];
        }
        
    } else {
        for (size_t i = 0; i != 0; i--)
        {
            dst[i-1] = src[i-1];
        }
        
    }
    return dstptr;
}

int memcmp(const void* aptr, const void* bptr, size_t size) {
    const unsigned char* a = (const unsigned char*) aptr;
    const unsigned char* b = (const unsigned char*) bptr;
    for (size_t i = 0; i < size; i++)
    {
        if(a[i] < b[i]){
            return -1;
        } else if (b[i] < a[i]){
            return 1;
        }
    }
    return 0;
    
}

void memcpy(void* dstptr, const void* srcptr, size_t size) {
    const uint8_t* src = (const uint8_t*) srcptr;

    uint8_t* dst = (uint8_t*) dstptr;
    for (; size != 0; size--)
    {
        *dst++ = *src++;
    }
    //return dst;
}

void *memset(void *pointer, uint32_t value, size_t size)
{
    uint8_t *buffer_pointer = (uint8_t *)pointer;

    for (size_t i = 0; i < size; i++)
        *buffer_pointer++ = value;

    return pointer;
}

void outportb (unsigned short _port, unsigned char _data)
{
    __asm__ __volatile__ ("outb %1, %0" : : "dN" (_port), "a" (_data));
}

unsigned char inportb (unsigned short _port)
{
    unsigned char rv;
    __asm__ __volatile__ ("inb %1, %0" : "=a" (rv) : "dN" (_port));
    return rv;
}


uint16_t inportw(uint16_t _port) {
    uint16_t rv;
    asm volatile ("inw %1, %0" : "=a" (rv) : "dN" (_port));
    return rv;
}

/*
 * Write 2 bytes
 * */
void outportw(uint16_t _port, uint16_t _data) {
    asm volatile ("outw %1, %0" : : "dN" (_port), "a" (_data));
}

/*
 * Readt 4 bytes
 * */
uint32_t inportl(uint16_t _port) {
    uint32_t rv;
    asm volatile ("inl %%dx, %%eax" : "=a" (rv) : "dN" (_port));
    return rv;
}

/*
 * Write 4 bytes
 * */
void outportl(uint16_t _port, uint32_t _data) {
    asm volatile ("outl %%eax, %%dx" : : "dN" (_port), "a" (_data));
}

void outportsm(unsigned short  port, unsigned char * data, unsigned long   size) {
	asm volatile ("rep outsw" : "+S" (data), "+c" (size) : "d" (port));
}

/*
 * Input multiple sets of shorts
 */
void inportsm(unsigned short  port, unsigned char * data, unsigned long   size) {
	asm volatile ("rep insw" : "+D" (data), "+c" (size) : "d" (port) : "memory");
}


int strncmp(char *str1, char *str2, int len){
    for (size_t i = 0; i < len; i++)
    {
        if(*str1++!=*str2++) return 1;
    }
    return 0;
    
}
void io_wait() {
     inportw(0x80);
}

size_t strspn(const char * str,const char * accept) {
	const char * ptr;
	const char * acc;
	size_t size = 0;
	for (ptr = str; *ptr != '\0'; ++ptr) {
		for (acc = accept; *acc != '\0'; ++acc) {
			if (*ptr == *acc) {
				break;
			}
		}
		if (*acc == '\0') {
			return size;
		} else {
			++size;
		}
	}
	return size;
}


char * strpbrk(const char * str, const char * accept) {
	while (*str != '\0') {
		const char *acc = accept;
		while (*acc != '\0') {
			if (*acc++ == *str) {
				return (char *) str;
			}
		}
		++str;
	}
	return nullptr;
}


size_t lfind(const char * str,const char accept) {
	size_t i = 0;
	while ( str[i] != accept) {
		i++;
	}
	return (size_t)(str) + i;
}

size_t rfind(const char * str,const char accept) {
	size_t i = strlen(str) - 1;
	while (str[i] != accept) {
		if (i == 0) return UINT32_MAX;
		i--;
	}
	return (size_t)(str) + i;
}


char * strtok_r(char * str, const char * delim, char ** saveptr) {
	char * token;
	if (str == nullptr) {
		str = *saveptr;
	}
	str += strspn(str, delim);
	if (*str == '\0') {
		*saveptr = str;
		return nullptr;
	}
	token = str;
	str = strpbrk(token, delim);
	if (str == nullptr) {
		*saveptr = (char *)lfind(token, '\0');
	} else {
		*str = '\0';
		*saveptr = str + 1;
	}
	return token;
}