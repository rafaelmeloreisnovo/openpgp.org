# RAFCODEΦ Bare-metal Octets V1 — independent freestanding C11

## Scope and provenance

New, independently authored MIT source in this directory only. This identical,
self-contained module is mirrored in **rafaelmeloreisnovo/CryptoSwift_RmR** and
**rafaelmeloreisnovo/openpgp.org**, with exact matching Git blob SHA on each
source file to establish parity. Neither repository imports code from the other.

The inherited CryptoSwift source/README/LICENSE and OpenPGP site/theme/assets
remain governed by their original owners and notices. This contribution
**does not relicense either repository**. Preserve upstream history, attribution
and all existing copyright notices.

## API

- `read_be32/write_be32`: checked-length deterministic big-endian wire encoding.
- `xor`: bounded byte-array utility; length and pointer checks, exact full alias supported.
- `different`: 0 equal, 1 different, -1 invalid input; length is public.
- `volatile_zero`: volatile clearing; **not** certified guaranteed key erasure.
- No hosted includes, heap, libc, syscalls, networking, logging or third-party code
  in the C11 core. Build/CI/test shell and compilers remain external **build-time**
  dependencies. No embedded OS runtime is pulled into the linked probe.

## Tests

Run `sh rafcode/baremetal_v1/verify_baremetal.sh` from repository root.

The script compiles GCC/Clang × O0/O2/Os host policy tests (6), and cross
compiles and links ARMv7A-none-eabi / AArch64-none-elf × O2/Os (4). Linkage
checks use `-nostdlib` and reject unresolved external symbols and dynamic
library dependencies.

The linker script uses synthetic link address 0x10000; **DO NOT FLASH or BOOT**
the resulting probe. It does not supply a vector table, stack setup, MMU,
runtime initialization, firmware loading protocol or device-specific boot ROM
contract. Android/Termux uses a Linux userspace ABI and is not bare-metal.

No AES/RSA/PGP implementations, key creation, encryption, post-quantum
security, timing side-channel resistance, secure erasure, speedup, ARM device
execution or certified cryptography are claimed.

## Compiler controls

Compilation, flags and link script affect machine code. Ordinary comments
do not. No text in a source comment is treated as an executable instruction.
No inline ASM/hex hand-optimization is enabled until there is a reproducible
baseline, disassembly comparison, adversarial correctness test and rollback.

## Gates

`SOURCE != ARTIFACT != EXECUTION != EVIDENCE != CLAIM`.
Local compiler evidence is PASS_SCOPED, hardware is TOKEN_VAZIO.
Human code/license review, scoped exact-head CI and physical device testing
are separate gates. This is a reversible DRAFT/HOLD contribution.
