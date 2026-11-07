/* SPDX-License-Identifier: MIT */
#ifndef RAFCODE_OPENPGP_PACKET_H
#define RAFCODE_OPENPGP_PACKET_H

#include <stddef.h> /* Freestanding C11 compiler header; no libc linkage. */

#ifdef __cplusplus
extern "C" {
#endif

enum rafpgp_status {
    RAFPGP_OK = 0,
    RAFPGP_NEED_MORE = 1,
    RAFPGP_UNSUPPORTED = 2,
    RAFPGP_MALFORMED = 3,
    RAFPGP_BAD_ARGUMENT = 4
};

struct rafpgp_header {
    unsigned char tag;
    size_t header_octets;
    unsigned long long body_octets;
};

/* RFC 9580, new-format, definite-length header subset only.
 * No crypto, key validation, old-format packets, or partial-body streams.
 * On non-OK status, *out remains unchanged.
 */
enum rafpgp_status rafpgp_parse_header(const unsigned char *source,
                                       size_t source_octets,
                                       struct rafpgp_header *out);

/* Require a complete single packet. On non-OK, *consumed is unchanged.
 * Additional trailing bytes are allowed for the caller to parse separately.
 */
enum rafpgp_status rafpgp_require_complete(const unsigned char *source,
                                           size_t source_octets,
                                           struct rafpgp_header *out,
                                           size_t *consumed);

#ifdef __cplusplus
}
#endif
#endif
