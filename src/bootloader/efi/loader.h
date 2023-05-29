#ifndef VOS_EFI_LOADER_H
#define VOS_EFI_LOADER_H

typedef struct {
  EFI_SYSTEM_TABLE* st;
  EFI_HANDLE img_h;
  EFI_LOADED_IMAGE_PROTOCOL* img_p;

  EFI_HANDLE esp_h;
  EFI_SIMPLE_FILE_SYSTEM_PROTOCOL* esp_p;

  EFI_FILE_PROTOCOL* root_fs;
} FileLoader;

EFI_STATUS initLoader(FileLoader* fl, EFI_SYSTEM_TABLE* st, EFI_HANDLE handle);

#endif
