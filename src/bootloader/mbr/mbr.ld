OUTPUT_FORMAT("binary")
OUTPUT_ARCH(i386:x86-64)

SECTIONS
{
  .boot 0x7C00 : {
    *mbr.nasm.o (.text)
    *mbr.nasm.o (.data)
  } =0x0

  .text : {
    *(.text)
  } =0x0

  .data : {
    *(.data)

    . = 0x1FE;

    BYTE(0x55);
    BYTE(0xAA);
  } =0x0

  /DISCARD/ : {
    *(.note*)
    *(.eh_frame)
  }
}
