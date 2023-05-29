#ifndef VOS_EFI_LOG_H
#define VOS_EFI_LOG_H

#include <efi.h>

void log(EFI_SYSTEM_TABLE* st, const char* fmt, ...);

void err(EFI_SYSTEM_TABLE* st, const char* fmt, ...);

#endif // VOS_EFI_LOG_H
