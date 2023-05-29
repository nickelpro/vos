set(flags

  -ffreestanding      # Don't make assumptions about stdlib availability

  -mno-red-zone       # Adjust rsp for all local variables, don't assume
                      # 128-byte safety zone

  -fno-stack-check    # Don't generate stack checking code


  -mgeneral-regs-only # Don't use SSE/SSE2/MMX registers

  -mcmodel=large      # Don't assume all code/data is in lower 2GB of address
                      # space
                    
  -fno-pic            # Don't try to generate a global offset table
)

find_file(efi_header_file efi.h HINTS /include /usr/include /usr/include/efi)
cmake_path(GET efi_header_file PARENT_PATH efi_include)

function(vos_add_freestanding_executable target)
  add_executable(${target})
  target_compile_options(${target}
    PRIVATE $<$<COMPILE_LANGUAGE:C>:${flags}>
  )
  target_link_options(${target} PRIVATE -nostdlib)
endfunction()

function(vos_target_linker_script target script)
  target_link_options(${target}
    PRIVATE -T ${CMAKE_CURRENT_SOURCE_DIR}/${script}
  )
  set_target_properties(${target}
    PROPERTIES LINK_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${script}
  )
endfunction()

function(vos_add_efi_executable target)
  add_executable(${target})
  target_compile_options(${target} PRIVATE
    -ffreestanding
    -fshort-wchar
    -mno-red-zone
    -mgeneral-regs-only
    -target x86_64-pc-win32-coff
  )
  target_include_directories(${target} PRIVATE ${efi_include})
  target_link_options(${target} PRIVATE
    -target x86_64-pc-win32-coff
    -fuse-ld=lld-link
    -nostdlib
    -Wl,-subsystem:efi_application
    -Wl,-entry:efi_main
  )
endfunction()
