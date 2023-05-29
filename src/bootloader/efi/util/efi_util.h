#ifndef VOS_EFI_EFI_H
#define VOS_EFI_EFI_H

#include <efi.h>

#define CHK(val, label)                                                        \
  do {                                                                         \
    if((val) != EFI_SUCCESS)                                                   \
      goto label;                                                              \
  } while(0)

static inline char* getErrStr(EFI_STATUS err) {
  switch(err) {
    case EFI_SUCCESS:
      return "Success";
    case EFI_LOAD_ERROR:
      return "Load Error";
    case EFI_INVALID_PARAMETER:
      return "Invalid Parameter";
    case EFI_UNSUPPORTED:
      return "Unsupported";
    case EFI_BAD_BUFFER_SIZE:
      return "Bad Buffer Size";
    case EFI_BUFFER_TOO_SMALL:
      return "Buffer Too Small";
    case EFI_NOT_READY:
      return "EFI Not Ready";
    case EFI_DEVICE_ERROR:
      return "Device Error";
    case EFI_WRITE_PROTECTED:
      return "Write Protected";
    case EFI_OUT_OF_RESOURCES:
      return "Out of Resources";
    case EFI_VOLUME_CORRUPTED:
      return "Volume Corrupted";
    case EFI_VOLUME_FULL:
      return "Volume Full";
    case EFI_NO_MEDIA:
      return "No Media";
    case EFI_MEDIA_CHANGED:
      return "Media Changed";
    case EFI_NOT_FOUND:
      return "Not Found";
    case EFI_ACCESS_DENIED:
      return "Access Denied";
    case EFI_NO_RESPONSE:
      return "No Response";
    case EFI_NO_MAPPING:
      return "No Mapping";
    case EFI_TIMEOUT:
      return "Timeout";
    case EFI_NOT_STARTED:
      return "Not Started";
    case EFI_ALREADY_STARTED:
      return "Already Started";
    case EFI_ABORTED:
      return "Aborted";
    case EFI_ICMP_ERROR:
      return "ICMP Error";
    case EFI_TFTP_ERROR:
      return "TFTP Error";
    case EFI_PROTOCOL_ERROR:
      return "Protocol Error";
    case EFI_INCOMPATIBLE_VERSION:
      return "Incompatible Version";
    case EFI_SECURITY_VIOLATION:
      return "Security Violation";
    case EFI_CRC_ERROR:
      return "CRC Error";
    case EFI_END_OF_MEDIA:
      return "End of Media";
    case EFI_END_OF_FILE:
      return "End of File";
    case EFI_INVALID_LANGUAGE:
      return "Invalid Language";
    case EFI_COMPROMISED_DATA:
      return "Compromised Data";
  }
  return "Unknown Error";
}

static inline char* getMemTypeStr(EFI_MEMORY_TYPE type) {
  switch(type) {
    case EfiReservedMemoryType:
      return "Reserved";
    case EfiLoaderCode:
      return "Loader Code";
    case EfiLoaderData:
      return "Loader Data";
    case EfiBootServicesCode:
      return "Boot Services Code";
    case EfiBootServicesData:
      return "Boot Services Data";
    case EfiRuntimeServicesCode:
      return "Runtime Services Code";
    case EfiRuntimeServicesData:
      return "Runtime Services Data";
    case EfiConventionalMemory:
      return "Conventional Memory";
    case EfiUnusableMemory:
      return "Unusable Memory";
    case EfiACPIReclaimMemory:
      return "ACPI Reclaim Memory";
    case EfiACPIMemoryNVS:
      return "ACPI Memory NVS";
    case EfiMemoryMappedIO:
      return "Memory Mapped IO";
    case EfiMemoryMappedIOPortSpace:
      return "Memory Mapped IO Port Space";
    case EfiPalCode:
      return "PAL Code";
    default:
      break;
  }
  return "Unknown Memory Type";
}


static inline void* getSP() {
  size_t sp;
  asm volatile("mov %0, rsp" : "=r"(sp));
  return (void*) sp;
}

static inline EFI_STATUS openProto(EFI_SYSTEM_TABLE* st, EFI_HANDLE handle,
    EFI_GUID guid, void* intf) {
  return st->BootServices->HandleProtocol(handle, &guid, intf);
}

static inline EFI_STATUS openVolume(EFI_SIMPLE_FILE_SYSTEM_PROTOCOL* p,
    EFI_FILE_PROTOCOL** fs) {
  return p->OpenVolume(p, fs);
}

static inline EFI_STATUS getMemMapSize(EFI_SYSTEM_TABLE* st, uint64_t* size) {
  *size = 0;
  uint64_t a;
  EFI_STATUS s;
  s = st->BootServices->GetMemoryMap(size, NULL, &a, &a, (uint32_t*) &a);
  if(s == EFI_BUFFER_TOO_SMALL)
    return EFI_SUCCESS;
  return s;
}

static inline EFI_STATUS getMemMap(EFI_SYSTEM_TABLE* st, uint64_t* size,
    void* buf, uint64_t* map_key, uint64_t* desc_size, uint32_t* desc_ver) {
  return st->BootServices->GetMemoryMap(size, buf, map_key, desc_size,
      desc_ver);
}

static inline EFI_STATUS allocPool(EFI_SYSTEM_TABLE* st, uint64_t size,
    void* buf_ptr) {
  return st->BootServices->AllocatePool(EfiLoaderData, size, buf_ptr);
}

static inline void waitForInput(EFI_SYSTEM_TABLE* st) {
  uint64_t i;
  st->ConIn->Reset(st->ConIn, FALSE);
  st->BootServices->WaitForEvent(1, &st->ConIn->WaitForKey, &i);
}

#endif
