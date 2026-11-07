#!/bin/sh
# SPDX-License-Identifier: MIT
set -eu
cd "$(dirname "$0")"
tmp="$(mktemp -d)"
trap 'rm -rf "$tmp"' EXIT HUP INT TERM
run=0
for cc in gcc clang; do
    if ! command -v "$cc" >/dev/null 2>&1; then
        echo "SKIP $cc (not installed)"
        continue
    fi
    for opt in -O0 -O2 -Os; do
        "$cc" -std=c11 -ffreestanding -fno-builtin -fno-stack-protector \
            -Wall -Wextra -Werror -Wpedantic -Wconversion -Wsign-conversion \
            "$opt" -Iinclude -c src/rafpgp_packet.c -o "$tmp/core.o"
        if nm -u "$tmp/core.o" | grep . >/dev/null 2>&1; then
            echo "FAIL: undefined core symbols ($cc $opt)" >&2
            exit 1
        fi
        "$cc" -std=c11 -Wall -Wextra -Werror -Wpedantic -Wconversion -Wsign-conversion \
            "$opt" -Iinclude src/rafpgp_packet.c tests/test_packet.c -o "$tmp/test"
        "$tmp/test"
        run=$((run + 1))
        echo "PASS host: $cc $opt"
    done
done
[ "$run" -gt 0 ] || { echo 'FAIL: no host C compiler' >&2; exit 1; }
if command -v clang >/dev/null 2>&1; then
    for target in armv7a-none-eabi aarch64-none-elf; do
        clang -target "$target" -std=c11 -ffreestanding -fno-builtin \
            -fno-stack-protector -Wall -Wextra -Werror -Wpedantic \
            -Wconversion -Wsign-conversion -O2 -Iinclude \
            -c src/rafpgp_packet.c -o "$tmp/cross.o"
        if nm -u "$tmp/cross.o" | grep . >/dev/null 2>&1; then
            echo "FAIL: undefined cross core symbols ($target)" >&2
            exit 1
        fi
        echo "PASS cross object: $target (not device execution)"
    done
fi
printf 'PASS_SCOPED: %s hosted matrix combinations; core has no undefined symbols\n' "$run"
