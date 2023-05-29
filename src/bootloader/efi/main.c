#include <efi.h>

#include "loader.h"
#include "util/efi_util.h"
#include "util/log.h"

static const uint16_t kernel_path[] = u"vos\\kernel";

void log_descriptor(EFI_SYSTEM_TABLE* st, EFI_MEMORY_DESCRIPTOR* desc) {
  log(st, "Physical: %llx, Virtual: %llx\r\n\tType: %s\r\n\tPages: %llu\r\n",
      desc->PhysicalStart, desc->VirtualStart, getMemTypeStr(desc->Type),
      desc->NumberOfPages);
}

EFI_STATUS efi_main(EFI_HANDLE handle, EFI_SYSTEM_TABLE* st) {
  uint64_t ev_index;
  FileLoader fl;
  EFI_STATUS s = EFI_SUCCESS;

  void* sp = getSP();

  log(st, "Initializing file loader...\r\n");
  CHK(s = initLoader(&fl, st, handle), end);

  uint64_t size;
  CHK(s = getMemMapSize(st, &size), end);

  // Allocating memory increases the size of the memory map
  // so allocate EVEN MORE memory
  size += 512;

  log(st, "Allocating memory...\r\n");
  char* buf;
  CHK(s = allocPool(st, size, &buf), end);

  log(st, "Getting memory map...\r\n");
  uint64_t map_key, desc_size;
  uint32_t desc_ver;
  CHK(s = getMemMap(st, &size, buf, &map_key, &desc_size, &desc_ver), end);

  char* end = buf + size;
  for(char* p = buf; p < end; p += desc_size)
    log_descriptor(st, (EFI_MEMORY_DESCRIPTOR*) p);

  log(st, "Stack pointer is at: %p\r\n", sp);

end:
  if(s != EFI_SUCCESS)
    err(st, "Operation failed with err: %s\r\n", getErrStr(s));

  waitForInput(st);
  return s;
}
