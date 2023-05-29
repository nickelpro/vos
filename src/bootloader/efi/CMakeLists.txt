if(NOT(CMAKE_C_COMPILER_ID STREQUAL "Clang"))
  message("EFI Applications can only be built with clang, disabling target")
  return()
endif()

vos_add_efi_executable(vos_efi)
target_sources(vos_efi PRIVATE main.c loader.c)
target_compile_options(vos_efi PRIVATE -masm=intel)
set_target_properties(vos_efi PROPERTIES OUTPUT_NAME bootx64.efi)

add_subdirectory(util)
