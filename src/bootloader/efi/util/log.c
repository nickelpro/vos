#include <stdarg.h>

#include "lib.h"
#include <efi.h>

void print(EFI_SIMPLE_TEXT_OUT_PROTOCOL* out, const char* fmt, va_list args) {
  uint16_t msg[512];

  vsnprintf(msg, sizeof(msg), fmt, args);
  out->OutputString(out, msg);
}

void log(EFI_SYSTEM_TABLE* st, const char* fmt, ...) {
  va_list args;

  va_start(args, fmt);
  print(st->ConOut, fmt, args);
  va_end(args);
}

void err(EFI_SYSTEM_TABLE* st, const char* fmt, ...) {
  va_list args;

  va_start(args, fmt);
  print(st->StdErr, fmt, args);
  va_end(args);
}
