#ifndef _STDINT
#define _STDINT

//Unsigned first
typedef unsigned long long uint64_t;
typedef unsigned long uint32_t;
typedef unsigned short uint16_t;
typedef unsigned char uint8_t;
typedef unsigned long size_t;


//Signed
typedef signed long long int64_t;
typedef signed long int32_t;
typedef signed short int16_t;
typedef signed char int8_t;

#define USHRT_MAX 65535
#define UINT32_MAX 4294967295


#ifndef _UINTPTR_T
typedef __UINTPTR_TYPE__ uintptr_t;
#define _UINTPTR_T
#endif

/*typedef unsigned short bool;

#define true 1
#define false 0
*/
#endif