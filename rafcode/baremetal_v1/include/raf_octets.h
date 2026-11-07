/* SPDX-License-Identifier: MIT
 * RAFCODE-PHI original: freestanding binary I/O, no crypto primitives.
 */
#ifndef RAF_OCTETS_H
#define RAF_OCTETS_H
#ifdef __cplusplus
extern "C" {
#endif

enum raf_octets_status {
    RAF_OCTETS_OK = 0,
    RAF_OCTETS_RANGE = 1,
    RAF_OCTETS_NULL = 2
};

/* Big-endian 32-bit wire encoding. Failure leaves output untouched. */
int raf_octets_read_be32(const unsigned char *src, unsigned long len,
                         unsigned int *value);
int raf_octets_write_be32(unsigned char *dst, unsigned long cap,
                          unsigned int value);

/* Permits dst == a or dst == b. Other partial overlaps are caller-invalid. */
int raf_octets_xor(unsigned char *dst, unsigned long cap,
                   const unsigned char *a, const unsigned char *b,
                   unsigned long len);

/* 0 means identical, 1 means different, -1 means bad pointer.
 * Iteration count depends on PUBLIC len, but timing is NOT certified. */
int raf_octets_different(const unsigned char *a, const unsigned char *b,
                         unsigned long len);

/* Volatile writes are intended to resist dead-store elimination;
 * this is NOT a verified secrets erasure guarantee on any target. */
int raf_octets_volatile_zero(void *dst, unsigned long len);
#ifdef __cplusplus
}
#endif
#endif
