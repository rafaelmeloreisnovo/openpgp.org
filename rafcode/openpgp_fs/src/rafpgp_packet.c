/* SPDX-License-Identifier: MIT */
#include "rafpgp_packet.h"
#include <limits.h> /* Freestanding compiler header. */
_Static_assert(CHAR_BIT == 8, "OpenPGP requires 8-bit octets");

enum rafpgp_status rafpgp_parse_header(const unsigned char *source,
                                       size_t source_octets,
                                       struct rafpgp_header *out)
{
    struct rafpgp_header result;
    unsigned int first;
    unsigned int code;
    if (source == (const unsigned char *)0 || out == (struct rafpgp_header *)0)
        return RAFPGP_BAD_ARGUMENT;
    if (source_octets == 0u)
        return RAFPGP_NEED_MORE;

    first = (unsigned int)source[0];
    if ((first & 0x80u) == 0u)
        return RAFPGP_MALFORMED;
    if ((first & 0x40u) == 0u)
        return RAFPGP_UNSUPPORTED; /* old packet-header format */
    result.tag = (unsigned char)(first & 0x3fu);
    if (result.tag == 0u)
        return RAFPGP_MALFORMED; /* reserved tag */
    if (source_octets < 2u)
        return RAFPGP_NEED_MORE;

    code = (unsigned int)source[1];
    if (code < 192u) {
        result.header_octets = 2u;
        result.body_octets = (unsigned long long)code;
    } else if (code < 224u) {
        if (source_octets < 3u)
            return RAFPGP_NEED_MORE;
        result.header_octets = 3u;
        result.body_octets = (unsigned long long)(((code - 192u) << 8) +
                             (unsigned int)source[2] + 192u);
    } else if (code < 255u) {
        return RAFPGP_UNSUPPORTED; /* partial body length requires streaming */
    } else {
        if (source_octets < 6u)
            return RAFPGP_NEED_MORE;
        result.header_octets = 6u;
        result.body_octets = ((unsigned long long)source[2] << 24) |
                             ((unsigned long long)source[3] << 16) |
                             ((unsigned long long)source[4] << 8) |
                             (unsigned long long)source[5];
    }
    *out = result;
    return RAFPGP_OK;
}

enum rafpgp_status rafpgp_require_complete(const unsigned char *source,
                                           size_t source_octets,
                                           struct rafpgp_header *out,
                                           size_t *consumed)
{
    struct rafpgp_header parsed;
    enum rafpgp_status status;
    if (consumed == (size_t *)0 || out == (struct rafpgp_header *)0)
        return RAFPGP_BAD_ARGUMENT;
    status = rafpgp_parse_header(source, source_octets, &parsed);
    if (status != RAFPGP_OK)
        return status;
    /* Subtract before comparing: no size_t sum/wrap with untrusted length. */
    if (parsed.body_octets > (unsigned long long)(source_octets - parsed.header_octets))
        return RAFPGP_NEED_MORE;
    *consumed = parsed.header_octets + (size_t)parsed.body_octets;
    *out = parsed;
    return RAFPGP_OK;
}
