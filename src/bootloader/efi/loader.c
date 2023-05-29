#include <efi.h>

#include "loader.h"
#include "util/efi_util.h"
#include "util/log.h"


EFI_STATUS initLoader(FileLoader* fl, EFI_SYSTEM_TABLE* st, EFI_HANDLE img_h) {

  fl->st = st;
  fl->img_h = img_h;

  EFI_STATUS s;
  s = openProto(st, img_h, (EFI_GUID) LOADED_IMAGE_PROTOCOL, &fl->img_p);

  if(s != EFI_SUCCESS) {
    err(st, "failed to open loaded image protocol\r\n");
    return s;
  }

  fl->esp_h = fl->img_p->DeviceHandle;
  s = openProto(st, fl->esp_h, (EFI_GUID) SIMPLE_FILE_SYSTEM_PROTOCOL,
      &fl->esp_p);

  if(s != EFI_SUCCESS) {
    err(st, "failed to open file protocol for ESP\r\n");
    return s;
  }

  s = openVolume(fl->esp_p, &fl->root_fs);

  if(s != EFI_SUCCESS) {
    err(st, "failed to open ESP file system volume\r\n");
    return s;
  }

  void* sp = getSP();
  log(st, "Stack pointer is at: %p\r\n", sp);

  return EFI_SUCCESS;
}
