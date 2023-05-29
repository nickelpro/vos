// https://github.com/krinkinmu/efi/blob/master/clib.c

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

#include "lib.h"

typedef enum {
  FMT_0,
  FMT_32,
  FMT_64,
} format_width;

typedef enum {
  FMT_NONE,
  FMT_I32,
  FMT_I64,
  FMT_U32,
  FMT_U64,
  FMT_STR,
  FMT_UEFI_STR,
  FMT_PERCENT,
  FMT_CHAR,
} format_type;

const uint8_t FMT_MINUS = 1;
const uint8_t FMT_PLUS = 2;
const uint8_t FMT_SPACE = 4;
const uint8_t FMT_BASE = 8;
const uint8_t FMT_ZERO = 16;

typedef struct {
  uint16_t* base;
  size_t size;
  size_t used;
} u16Buffer;

typedef struct {
  uint8_t flags;
  uint8_t width;
  uint8_t width_from_args;
  uint8_t base;
  format_width type_width;
  format_type type;
  size_t size;
} BufferFormat;

static void appendBuf(u16Buffer* buf, uint16_t c) {
  if(buf->used < buf->size) {
    buf->base[buf->used] = c;
    ++buf->used;
  }
}

static void copyBuf(u16Buffer* buf, const char* src, size_t size) {
  size_t rem = buf->size - buf->used;
  size_t to_copy = size < rem ? size : rem;

  uint16_t* start = buf->base + buf->used;
  for(size_t i = 0; i < to_copy; ++i)
    start[i] = src[i];
  buf->used += to_copy;
}

static void copyBuf16(u16Buffer* buf, const uint16_t* src, size_t size) {
  size_t rem = buf->size - buf->used;
  size_t to_copy = size < rem ? size : rem;

  uint16_t* start = buf->base + buf->used;
  for(size_t i = 0; i < to_copy; ++i)
    start[i] = src[i];
  buf->used += to_copy;
}

static void itoaBufferUnsigned(u16Buffer* buf, const BufferFormat* fmt,
    uint64_t val) {
  static char local_buf[32];
  static char* end = local_buf + sizeof(local_buf);
  char* cur = end;

  do {
    uint16_t rem = val % fmt->base;
    val /= fmt->base;

    if(rem < 10)
      *--cur = rem + '0';
    else
      *--cur = (rem - 10) + 'A';
  } while(val);

  copyBuf(buf, cur, end - cur);
}

static void itoaBuffer(u16Buffer* buf, const BufferFormat* fmt, int64_t val) {
  if(val < 0) {
    appendBuf(buf, '-');
    val = -val;
  }
  itoaBufferUnsigned(buf, fmt, val);
}

static const char* parseFlags(const char* str, BufferFormat* fmt) {
  while(*str) {
    switch(*str) {
      case '-':
        fmt->flags = FMT_MINUS;
        break;
      case '+':
        fmt->flags = FMT_PLUS;
        break;
      case ' ':
        fmt->flags = FMT_SPACE;
        break;
      case '#':
        fmt->flags = FMT_BASE;
        break;
      case '0':
        fmt->flags = FMT_ZERO;
        break;
      default:
        return str;
    }
    ++str;
  }
  return str;
}

static const char* parseWidth(const char* str, BufferFormat* fmt) {
  if(*str == '*') {
    fmt->width_from_args = 1;
    return str + 1;
  }

  while(isdigit(*str)) {
    fmt->width = fmt->width * 10 + (*str - '0');
    ++str;
  }

  return str;
}

static const char* parseTypeWidth(const char* str, BufferFormat* fmt) {
  switch(*str) {
    case 'z':
    case 't':
      fmt->type_width = FMT_64;
      return str + 1;

    case 'h':
      ++str;
      fmt->type_width = FMT_32;
      if(*str == 'h') {
        return str + 1;
      }
      return str;

    case 'l':
      ++str;
      if(*str == 'l') {
        fmt->type_width = FMT_64;
        return str + 1;
      }
      fmt->type_width = FMT_32;
      return str;
  }
  return str;
}

format_type adjustType(format_type type, format_width width) {
  if(type == FMT_I32) {
    if(width == FMT_32)
      return FMT_I32;
    return FMT_I64;
  }

  if(type == FMT_U32) {
    if(width == FMT_32)
      return FMT_U32;
    return FMT_U64;
  }

  return type;
}

static const char* parseSpec(const char* str, BufferFormat* fmt) {
  const char* cur = str;

  if(*cur == '%') {
    fmt->type = FMT_PERCENT;
    return cur + 1;
  }

  cur = parseTypeWidth(cur, fmt);

  switch(*cur) {
    case 'd':
    case 'i':
    case 'u':
      fmt->base = 10;
      break;
    case 'o':
      fmt->base = 8;
      break;
    case 'x':
    case 'X':
    case 'p':
      fmt->base = 16;
      break;
  }

  switch(*cur) {
    case 'd':
    case 'i':
      fmt->type = adjustType(FMT_I32, fmt->type_width);
      break;
    case 'u':
    case 'o':
    case 'x':
    case 'X':
      fmt->type = adjustType(FMT_U32, fmt->type_width);
      break;
    case 'c':
      fmt->type = FMT_CHAR;
      break;
    case 's':
      fmt->type = FMT_STR;
      break;
    case 'w':
      fmt->type = FMT_UEFI_STR;
      break;
    case 'p':
      fmt->type = FMT_U64;
      break;
    default:
      return str;
  }

  return cur + 1;
}

static int parseFormat(const char* str, BufferFormat* fmt) {
  const char* cur = str;
  const char* end;
  *fmt = (BufferFormat) {0};

  cur = parseFlags(cur, fmt);
  cur = parseWidth(cur, fmt);

  end = parseSpec(cur, fmt);
  if(cur == end)
    return -1;
  fmt->size = end - cur;
  return 0;
}

int vsnprintf(uint16_t* buffer, size_t size, const char* fmtstr, va_list args) {
  u16Buffer buf = {
      .base = buffer,
      .size = size - 1,
      .used = 0,
  };

  const char* cur = fmtstr;

  while(*cur) {
    const char* start = cur;
    const char* end = cur;

    BufferFormat fmt;
    uint64_t v64;
    uint32_t v32;

    while(*end && *end != '%')
      ++end;

    copyBuf(&buf, start, end - start);
    if(!*end) {
      cur = end;
      break;
    }

    int ret = parseFormat(end + 1, &fmt);
    if(ret)
      return ret;

    cur = end + 1 + fmt.size;

    if(fmt.width_from_args)
      fmt.width = (uint8_t) va_arg(args, int);


    switch(fmt.type) {
      case FMT_STR: {
        const char* str = va_arg(args, const char*);
        copyBuf(&buf, str, strlen(str));
        break;
      }
      case FMT_UEFI_STR: {
        const uint16_t* str = va_arg(args, const uint16_t*);
        copyBuf16(&buf, str, u16strlen(str));
        break;
      }
      case FMT_CHAR:
        v32 = va_arg(args, uint32_t);
        appendBuf(&buf, v32);
        break;
      case FMT_PERCENT:
        appendBuf(&buf, '%');
        break;
      case FMT_I32:
        v32 = va_arg(args, uint32_t);
        itoaBuffer(&buf, &fmt, (int64_t) v32);
        break;
      case FMT_I64:
        v64 = va_arg(args, uint64_t);
        itoaBuffer(&buf, &fmt, (int64_t) v64);
        break;
      case FMT_U32:
        v32 = va_arg(args, uint32_t);
        itoaBufferUnsigned(&buf, &fmt, v32);
        break;
      case FMT_U64:
        v64 = va_arg(args, uint64_t);
        itoaBufferUnsigned(&buf, &fmt, v64);
        break;
      default:
        return -1;
    }
  }

  buf.base[buf.used] = 0;
  return buf.used;
}

int u16snprintf(uint16_t* buffer, size_t size, const char* fmt, ...) {
  va_list args;
  int ret;

  va_start(args, fmt);
  ret = vsnprintf(buffer, size, fmt, args);
  va_end(args);
  return ret;
}

size_t strlen(const char* str) {
  const char* cur = str;
  while(*cur++) {}
  return cur - str - 1;
}

size_t u16strlen(const uint16_t* str) {
  const uint16_t* cur = str;
  while(*cur++) {}
  return cur - str - 1;
}

void* memset(void* p, unsigned char val, size_t size) {
  unsigned char* o = (unsigned char*) p;
  for(size_t i = 0; i < size; ++i)
    *o++ = val;
  return p;
}

void* memcpy(void* dst, const void* src, size_t size) {
  char* o = (char*) dst;
  char* i = (char*) src;
  for(size_t j = 0; j < size; ++j)
    *o++ = *i++;
  return dst;
}

int isdigit(int c) {
  return c >= '0' && c <= '9';
}

int isalpha(int c) {
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

int isalnum(int c) {
  return isdigit(c) || isalpha(c);
}

int isspace(int c) {
  return c == ' ' || c == '\r' || c == '\n' || c == '\t';
}
