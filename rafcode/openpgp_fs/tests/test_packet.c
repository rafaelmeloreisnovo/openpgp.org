/* SPDX-License-Identifier: MIT */
#include "rafpgp_packet.h"
#include <stdio.h> /* Test driver only, never linked to freestanding core. */

#define CHECK(x) do { if (!(x)) { (void)fprintf(stderr, "FAIL at line %d: %s\n", __LINE__, #x); return 1; } } while (0)

static int test_packets(void)
{
    const unsigned char literal[] = {0xcbu, 3u, 1u, 2u, 3u, 0u};
    const unsigned char two[] = {0xcbu, 192u, 0u};
    const unsigned char five[] = {0xcbu, 255u, 0u, 0u, 1u, 0u};
    const unsigned char rfc_1723[] = {0xcbu, 0xc5u, 0xfbu};
    const unsigned char rfc_100000[] = {0xcbu, 255u, 0u, 1u, 0x86u, 0xa0u};
    const unsigned char empty[] = {0xcbu, 0u};
    const unsigned char partial[] = {0xcbu, 224u};
    const unsigned char old[] = {0x80u, 0u};
    const unsigned char invalid[] = {0u, 0u};
    const unsigned char reserved[] = {0xc0u, 0u};
    const unsigned char high[] = {0xcbu, 255u, 255u, 255u, 255u, 255u};
    struct rafpgp_header h = {99u, 9u, 9u};
    size_t used = 555u;
    CHECK(rafpgp_require_complete(literal, sizeof literal, &h, &used) == RAFPGP_OK);
    CHECK(h.tag == 11u && h.header_octets == 2u && h.body_octets == 3u);
    CHECK(used == 5u); /* One trailing byte is not swallowed. */
    CHECK(rafpgp_parse_header(two, sizeof two, &h) == RAFPGP_OK);
    CHECK(h.header_octets == 3u && h.body_octets == 192u);
    CHECK(rafpgp_require_complete(two, sizeof two, &h, &used) == RAFPGP_NEED_MORE);
    CHECK(used == 5u);
    CHECK(rafpgp_parse_header(rfc_1723, sizeof rfc_1723, &h) == RAFPGP_OK);
    CHECK(h.body_octets == 1723u);
    CHECK(rafpgp_parse_header(rfc_100000, sizeof rfc_100000, &h) == RAFPGP_OK);
    CHECK(h.body_octets == 100000u);
    CHECK(rafpgp_require_complete(empty, sizeof empty, &h, &used) == RAFPGP_OK);
    CHECK(used == 2u && h.body_octets == 0u);
    CHECK(rafpgp_parse_header(five, sizeof five, &h) == RAFPGP_OK);
    CHECK(h.header_octets == 6u && h.body_octets == 256u);
    CHECK(rafpgp_parse_header(high, sizeof high, &h) == RAFPGP_OK);
    CHECK(h.body_octets == 4294967295ULL);
    CHECK(rafpgp_parse_header(partial, sizeof partial, &h) == RAFPGP_UNSUPPORTED);
    CHECK(rafpgp_parse_header(old, sizeof old, &h) == RAFPGP_UNSUPPORTED);
    CHECK(rafpgp_parse_header(invalid, sizeof invalid, &h) == RAFPGP_MALFORMED);
    CHECK(rafpgp_parse_header(reserved, sizeof reserved, &h) == RAFPGP_MALFORMED);
    CHECK(rafpgp_parse_header(literal, 1u, &h) == RAFPGP_NEED_MORE);
    CHECK(rafpgp_parse_header(two, 2u, &h) == RAFPGP_NEED_MORE);
    CHECK(rafpgp_parse_header(five, 5u, &h) == RAFPGP_NEED_MORE);
    CHECK(rafpgp_parse_header((const unsigned char *)0, 0u, &h) == RAFPGP_BAD_ARGUMENT);
    CHECK(rafpgp_parse_header(literal, sizeof literal, (struct rafpgp_header *)0) == RAFPGP_BAD_ARGUMENT);
    CHECK(rafpgp_require_complete(literal, sizeof literal, &h, (size_t *)0) == RAFPGP_BAD_ARGUMENT);
    CHECK(h.header_octets == 6u && h.body_octets == 4294967295ULL);
    return 0;
}

int main(void) { return test_packets(); }
