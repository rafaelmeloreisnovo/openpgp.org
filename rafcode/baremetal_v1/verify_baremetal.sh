#!/bin/sh
# SPDX-License-Identifier: MIT
set -eu
cd "$(dirname "$0")"
: "${TMPDIR:=/tmp}"
tmp=$(mktemp -d "${TMPDIR%/}/rafbm.XXXXXXXX")
trap 'rm -rf "$tmp"' EXIT HUP INT TERM
run=0
for cc in gcc clang; do
  command -v "$cc" >/dev/null 2>&1 || continue
  for opt in 0 2 s; do
    "$cc" -std=c11 -O"$opt" -ffreestanding -fno-builtin -fno-stack-protector \
      -Wall -Wextra -Werror -pedantic -Wconversion -Wsign-conversion \
      -Iinclude -c src/raf_octets.c -o "$tmp/octets.o"
    if nm -u "$tmp/octets.o" | grep .; then echo 'FAIL undefined core imports' >&2; exit 1; fi
    "$cc" -std=c11 -O"$opt" -Wall -Wextra -Werror -pedantic \
      -Iinclude src/raf_octets.c tests/test_octets.c -o "$tmp/host-test"
    "$tmp/host-test"
    run=$((run+1))
    echo "PASS host $cc O$opt"
  done
done
[ "$run" -gt 0 ] || { echo 'FAIL no host compilers' >&2; exit 1; }
command -v clang >/dev/null 2>&1 || { echo 'FAIL Clang required for cross gate' >&2; exit 1; }
command -v ld.lld >/dev/null 2>&1 || { echo 'FAIL LLD required for link gate' >&2; exit 1; }
for target in armv7a-none-eabi aarch64-none-elf; do
  for opt in 2 s; do
    clang -target "$target" -std=c11 -O"$opt" -ffreestanding \
      -fno-builtin -fno-stack-protector -fno-unwind-tables -fno-asynchronous-unwind-tables \
      -Wall -Wextra -Werror -pedantic -Wconversion -Wsign-conversion \
      -ffunction-sections -fdata-sections -Iinclude -c src/raf_octets.c -o "$tmp/octets.o"
    clang -target "$target" -std=c11 -O"$opt" -ffreestanding \
      -fno-builtin -fno-stack-protector -fno-unwind-tables -fno-asynchronous-unwind-tables \
      -Wall -Wextra -Werror -pedantic -Wconversion -Wsign-conversion \
      -ffunction-sections -fdata-sections -Iinclude -c baremetal/probe_start.c -o "$tmp/probe.o"
    if nm -u "$tmp/octets.o" | grep .; then echo "FAIL cross imports $target" >&2; exit 1; fi
    clang -target "$target" -fuse-ld=lld -nostdlib -Wl,--no-undefined \
      -Wl,--gc-sections -Wl,-T,baremetal/probe.ld \
      "$tmp/probe.o" "$tmp/octets.o" -o "$tmp/probe.elf"
    if nm -u "$tmp/probe.elf" | grep .; then echo "FAIL ELF imports $target" >&2; exit 1; fi
    if readelf -d "$tmp/probe.elf" 2>/dev/null | grep -E 'NEEDED|INTERP'; then
      echo "FAIL dynamic link dependency $target" >&2; exit 1
    fi
    readelf -h "$tmp/probe.elf" | grep -E 'Machine:' | tr -s ' '
    echo "PASS baremetal LINK-ONLY $target O$opt (not boot/hardware)"
  done
done
echo "PASS_SCOPED: host=${run}; cross_link=4; no libc/imports at checked ELF"
