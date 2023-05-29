vos_add_freestanding_executable(mbr_bootloader)
vos_target_linker_script(mbr_bootloader mbr.ld)
target_sources(mbr_bootloader PRIVATE mbr.c mbr.nasm)
target_include_directories(mbr_bootloader PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
