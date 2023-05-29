#ifndef VOS_EFI_LIB_H
#define VOS_EFI_LIB_H

#include <stddef.h>
#include <stdint.h>

size_t strlen(const char* str);

size_t u16strlen(const uint16_t* str);

void* memset(void* p, unsigned char val, size_t size);

void* memcpy(void* dst, const void* src, size_t size);

int isdigit(int c);

int isalpha(int c);

int isalnum(int c);

int isspace(int c);

int vsnprintf(uint16_t* buffer, size_t size, const char* fmtstr, va_list args);

int u16snprintf(uint16_t* buffer, size_t size, const char* fmt, ...);

#endif // VOS_EFI_LIB_H
