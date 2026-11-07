#!/bin/sh
# SPDX-License-Identifier: MIT
# Fail closed if toolchain probes or symbol inspection cannot run.
set -eu
LC_ALL=C
export LC_ALL
umask 077
cd "$(dirname "$0")"
for tool in gcc clang nm mktemp; do
    if ! command -v "$tool" >/dev/null 2>&1; then
        echo "FAIL_TOOLCHAIN: required verifier $tool unavailable" >&2
        exit 1
    fi
done
tmp="$(mktemp -d)"
trap 'rm -rf "$tmp"' EXIT HUP INT TERM
check_symbols() {
    # Both unsuccessful nm and undefined symbols are hard failures.
    if ! nm -u "$1" > "$tmp/undefined.txt"; then
        echo "FAIL_PROVENANCE: nm could not inspect $1" >&2
        exit 1
    fi
    if [ -s "$tmp/undefined.txt" ]; then
        echo "FAIL_IMPORTS: unresolved core symbols in $1" >&2
        cat "$tmp/undefined.txt" >&2
        exit 1
    fi
}
run=0
for cc in gcc clang; do
    for opt in -O0 -O2 -Os; do
        "$cc" -std=c11 -ffreestanding -fno-builtin -fno-stack-protector \
            -Wall -Wextra -Werror -Wpedantic -Wconversion -Wsign-conversion \
            "$opt" -Iinclude -c src/rafpgp_packet.c -o "$tmp/core.o"
        check_symbols "$tmp/core.o"
        for suite in test_packet test_adversarial; do
            "$cc" -std=c11 -Wall -Wextra -Werror -Wpedantic \
                -Wconversion -Wsign-conversion "$opt" -Iinclude \
                src/rafpgp_packet.c "tests/$suite.c" -o "$tmp/$suite"
            "$tmp/$suite"
        done
        run=$((run + 1))
        echo "PASS_HOST: $cc $opt both suites"
    done
    "$cc" -std=c11 -O1 -g -fno-omit-frame-pointer -fsanitize=address,undefined \
        -Wall -Wextra -Werror -Wpedantic -Wconversion -Wsign-conversion \
        -Iinclude src/rafpgp_packet.c tests/test_adversarial.c -o "$tmp/sanitized"
    "$tmp/sanitized"
    echo "PASS_SANITIZER: $cc ASan+UBSan (host only)"
done
for target in armv7a-none-eabi aarch64-none-elf; do
    clang -target "$target" -std=c11 -ffreestanding -fno-builtin \
        -fno-stack-protector -Wall -Wextra -Werror -Wpedantic \
        -Wconversion -Wsign-conversion -O2 -Iinclude \
        -c src/rafpgp_packet.c -o "$tmp/cross.o"
    check_symbols "$tmp/cross.o"
    echo "PASS_CROSS_OBJECT: $target (no device run)"
done
[ "$run" -eq 6 ] || { echo 'FAIL_MATRIX: expected six host combinations' >&2; exit 1; }
echo 'PASS_SCOPED: 6 host combinations + 2 host sanitizers + 2 cross objects; no unresolved core symbols'
