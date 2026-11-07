#!/bin/sh
# SPDX-License-Identifier: MIT
# Fail-closed scoped CI: hosted tests 6/6 + synthetic ARM ELF links 4/4.
set -eu
cd "$(dirname "$0")"
: "${TMPDIR:=/tmp}"
tmp=$(mktemp -d "${TMPDIR%/}/rafbm.XXXXXXXX")
trap 'rm -rf "$tmp"' EXIT HUP INT TERM
for tool in gcc clang nm readelf ld.lld; do
  if ! command -v "$tool" >/dev/null 2>&1; then
    echo "BLOCKED: required CI build tool missing: $tool" >&2; exit 1
  fi
done
printf 'TOOLCHAIN clang='; clang --version | sed -n '1p'
printf 'TOOLCHAIN lld='; ld.lld --version
run=0
for cc in gcc clang; do
  for opt in 0 2 s; do
    "$cc" -std=c11 -O"$opt" -ffreestanding -fno-builtin -fno-stack-protector \
      -Wall -Wextra -Werror -pedantic -Wconversion -Wsign-conversion \
      -Iinclude -c src/raf_octets.c -o "$tmp/octets.o"
    if nm -u "$tmp/octets.o" | grep .; then echo 'FAIL undefined core symbols' >&2; exit 1; fi
    "$cc" -std=c11 -O"$opt" -Wall -Wextra -Werror -pedantic \
      -Iinclude src/raf_octets.c tests/test_octets.c -o "$tmp/host-test"
    "$tmp/host-test"
    run=$((run+1))
    echo "PASS host $cc O$opt"
  done
done
[ "$run" -eq 6 ] || { echo "FAIL host matrix incomplete $run/6" >&2; exit 1; }
cross=0
for target in armv7a-none-eabi aarch64-none-elf; do
  case "$target" in
    armv7a-none-eabi) want='ARM' ;;
    aarch64-none-elf) want='AArch64' ;;
    *) echo 'BLOCKED: unexpected target' >&2; exit 1 ;;
  esac
  for opt in 2 s; do
    clang -target "$target" -std=c11 -O"$opt" -ffreestanding \
      -fno-builtin -fno-stack-protector -fno-unwind-tables -fno-asynchronous-unwind-tables \
      -Wall -Wextra -Werror -pedantic -Wconversion -Wsign-conversion \
      -ffunction-sections -fdata-sections -Iinclude -c src/raf_octets.c -o "$tmp/octets.o"
    clang -target "$target" -std=c11 -O"$opt" -ffreestanding \
      -fno-builtin -fno-stack-protector -fno-unwind-tables -fno-asynchronous-unwind-tables \
      -Wall -Wextra -Werror -pedantic -Wconversion -Wsign-conversion \
      -ffunction-sections -fdata-sections -Iinclude -c baremetal/probe_start.c -o "$tmp/probe.o"
    if nm -u "$tmp/octets.o" | grep .; then echo "FAIL core imports $target" >&2; exit 1; fi
    clang -target "$target" -fuse-ld=lld -nostdlib -Wl,--no-undefined \
      -Wl,--gc-sections -Wl,-T,baremetal/probe.ld \
      "$tmp/probe.o" "$tmp/octets.o" -o "$tmp/probe.elf"
    if nm -u "$tmp/probe.elf" | grep .; then echo "FAIL ELF imports $target" >&2; exit 1; fi
    if readelf -d "$tmp/probe.elf" | grep -E 'NEEDED|INTERP'; then
      echo "FAIL dynamic dependencies $target" >&2; exit 1
    fi
    if readelf -l "$tmp/probe.elf" | grep 'INTERP'; then
      echo "FAIL dynamic interpreter $target" >&2; exit 1
    fi
    if ! readelf -h "$tmp/probe.elf" | grep -Eq 'Type:[[:space:]]*EXEC'; then
      echo "FAIL non-executable ELF type $target" >&2; exit 1
    fi
    if ! readelf -h "$tmp/probe.elf" | grep -Eq "Machine:[[:space:]]*$want[[:space:]]*$"; then
      echo "FAIL machine mismatch $target" >&2; exit 1
    fi
    cross=$((cross+1))
    echo "PASS baremetal LINK-ONLY $target O$opt (not boot/device)"
  done
done
[ "$cross" -eq 4 ] || { echo "FAIL cross matrix incomplete $cross/4" >&2; exit 1; }
echo "PASS_SCOPED: host=$run/6 cross_link=$cross/4, exact native/cross ELF checks; physical_device=TOKEN_VAZIO"
