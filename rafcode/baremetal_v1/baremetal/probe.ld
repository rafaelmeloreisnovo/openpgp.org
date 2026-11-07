/* SPDX-License-Identifier: MIT
 * Synthetic link-only memory map; 0x10000 is NOT a real board address.
 * Do not flash or execute on physical hardware.
 */
ENTRY(_start)
SECTIONS {
  . = 0x10000;
  .text : { *(.text .text.*) }
  .rodata : { *(.rodata .rodata.*) }
  .data : { *(.data .data.*) }
  .bss : { *(.bss .bss.* COMMON) }
  /DISCARD/ : { *(.comment) *(.note*) *(.eh_frame*) }
}
