%include "segment_util.nasm"

extern thingy

section .data

align 16, db 0
gdt:
  d_std_gdt

gdt_desc:
  dw (gdt_desc - gdt)
  dd gdt

section .text
bits 16

  cli

  ; Overly defensive programming, assert 0x0000:0x7C00
  jmp 0x0000:FlushCS
FlushCS:

  ; More defensiveness, zero remaining segment registers
  xor ax, ax
  mov ds, ax
  mov es, ax
  mov ss, ax
  mov fs, ax
  mov gs, ax

  lgdt [gdt_desc]
  call thingy
