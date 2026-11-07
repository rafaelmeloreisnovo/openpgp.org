# RAFCODEΦ — Freestanding OpenPGP packet-header boundary (v1)

**Purpose:** a separately authored **C11 freestanding structural reader** of a deliberately narrow RFC 9580 subset (new-format, definite-length packet headers). No library runtime, allocation, syscalls, file access, signing, encryption, verification, key material handling, or network support in the core. The compiler's freestanding `<stddef.h>` supplies `size_t`; the test harness is hosted. The core does **not** implement OpenPGP cryptography or claim full RFC 9580 conformance.

## Contract and fail-closed behaviors

- Input is an untrusted octet span; absent pointers, invalid/reserved tags, old-format packet headers and partial body lengths are rejected with typed status.
- Bounds are checked before every read. `rafpgp_require_complete` compares against remaining bytes before any addition to avoid overflow.
- A failed call never mutates the output structure or `consumed`; trailing octets remain available to the caller.
- Only public wire framing data is processed, not secrets; **constant-time cryptographic claims do not apply.** An optimizer or assembler change is not a security proof.
- For unsupported formats, callers must use an independently validated interoperable implementation rather than ignore the status.

## Build and evidence

From the repository root: `sh rafcode/openpgp_fs/verify.sh`.

The script builds GCC/Clang `-O0/-O2/-Os` hosted test programs and checks freestanding core object undefined imports with `nm`. With Clang it also compiles ARMv7/AArch64 **object files**. These are host/cross build gates, **not** Android device evidence, cryptographic test vectors, release certification, performance benchmarks or an independent implementation of IETF RFC 9580. External compilers/CI are build tools, not runtime core dependencies.

### Next isolated gates

1. RFC 9580 packet-framing vector corpus and property/fuzz tests, with adverse length and truncation cases.
2. Independent implementation differential tests; review v4/v6 handling and all unsupported packet formats.
3. Compiler/target/codegen review, timing side-channel and ARM device checks **only if secret-bearing code is added**.
4. Content/license inventory for the inherited site and deploy preview (without changing upstream production).

Source ≠ artifact ≠ execution ≠ evidence ≠ security claim. `claim_allowed=false` until independent gates are documented.

See [LICENSE_SCOPE.md](LICENSE_SCOPE.md) for provenance and [RFC 9580](https://www.rfc-editor.org/rfc/rfc9580.html) for the normative packet format.

## V2 control gate and operational status

The companion [GATES_V2.md](GATES_V2.md) records typed unknowns, independent
promotions, merge-control limits, and rollback. The hardened verifier now
requires `gcc`, `clang`, `nm`, and `mktemp`; rejects failed `nm` invocations
instead of silently interpreting them as no unresolved symbols; exercises an
exhaustive 64×256 framing matrix and 131072 deterministic adverse spans under
both hosted compilers and ASan/UBSan. Hosted sanitizer PASS is **not** a
hardware, side-channel, interoperability, or crypto validation.
