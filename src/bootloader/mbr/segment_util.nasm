%define seg_present (1<<7)
%define seg_dpl_user (3<<5)
%define seg_type (1<<4)
%define seg_exec (1<<3)
%define seg_dc (1<<2)
%define seg_rw (1<<1)

%define seg_gran (1<<7)
%define seg_size (1<<6)
%define seg_long (1<<5)

%define d_null_seg_desc dq 0

%macro d_seg_desc 1
  dw 0xFFFF                      ; limit low
  dw 0x0000                      ; base low
  db 0x00                        ; base mid
  db %1                          ; access
  db (seg_gran | seg_size | 0xF) ; flags
  db 0x00                        ; base hi
%endmacro

%macro d_kernel_code_seg_desc 0
  d_seg_desc (seg_present | seg_type | seg_exec | seg_rw)
%endmacro

%macro d_kernel_data_seg_desc 0
  d_seg_desc (seg_present | seg_type | seg_rw)
%endmacro

%macro d_user_code_seg_desc 0
  d_seg_desc (seg_present | seg_dpl_user | seg_type | seg_exec | seg_rw)
%endmacro

%macro d_user_data_seg_desc 0
  d_seg_desc (seg_present | seg_dpl_user | seg_type | seg_rw)
%endmacro

%macro d_std_gdt 0
  d_null_seg_desc
  d_kernel_code_seg_desc
  d_kernel_data_seg_desc
  d_user_code_seg_desc
  d_user_data_seg_desc
%endmacro
