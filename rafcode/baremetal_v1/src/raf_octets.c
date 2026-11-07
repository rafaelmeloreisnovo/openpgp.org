/* SPDX-License-Identifier: MIT | RAFCODE-PHI original freestanding C11. */
#include "raf_octets.h"

_Static_assert(sizeof(unsigned int) == 4, "requires 32-bit unsigned int");
_Static_assert(sizeof(unsigned char) == 1, "requires byte-addressed chars");

int raf_octets_read_be32(const unsigned char *src, unsigned long len,
                         unsigned int *value) {
    unsigned int v;
    if (!value) return RAF_OCTETS_NULL;
    if (len < 4ul) return RAF_OCTETS_RANGE;
    if (!src) return RAF_OCTETS_NULL;
    v = ((unsigned int)src[0] << 24) |
        ((unsigned int)src[1] << 16) |
        ((unsigned int)src[2] << 8) | (unsigned int)src[3];
    *value = v;
    return RAF_OCTETS_OK;
}

int raf_octets_write_be32(unsigned char *dst, unsigned long cap,
                          unsigned int value) {
    if (cap < 4ul) return RAF_OCTETS_RANGE;
    if (!dst) return RAF_OCTETS_NULL;
    dst[0] = (unsigned char)(value >> 24);
    dst[1] = (unsigned char)(value >> 16);
    dst[2] = (unsigned char)(value >> 8);
    dst[3] = (unsigned char)value;
    return RAF_OCTETS_OK;
}

int raf_octets_xor(unsigned char *dst, unsigned long cap,
                   const unsigned char *a, const unsigned char *b,
                   unsigned long len) {
    unsigned long i;
    if (len > cap) return RAF_OCTETS_RANGE;
    if (len == 0ul) return RAF_OCTETS_OK;
    if (!dst || !a || !b) return RAF_OCTETS_NULL;
    for (i = 0ul; i < len; ++i) dst[i] = (unsigned char)(a[i] ^ b[i]);
    return RAF_OCTETS_OK;
}

int raf_octets_different(const unsigned char *a, const unsigned char *b,
                         unsigned long len) {
    unsigned long i;
    unsigned int diff = 0u;
    if (len == 0ul) return 0;
    if (!a || !b) return -1;
    for (i = 0ul; i < len; ++i) diff |= (unsigned int)(a[i] ^ b[i]);
    return (diff != 0u) ? 1 : 0;
}

int raf_octets_volatile_zero(void *dst, unsigned long len) {
    unsigned long i;
    volatile unsigned char *p;
    if (len == 0ul) return RAF_OCTETS_OK;
    if (!dst) return RAF_OCTETS_NULL;
    p = (volatile unsigned char *)dst;
    for (i = 0ul; i < len; ++i) p[i] = 0u;
    return RAF_OCTETS_OK;
}
