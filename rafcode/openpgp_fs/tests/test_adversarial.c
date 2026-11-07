/* SPDX-License-Identifier: MIT */
/* Exhaustive length-octet boundaries, independent length oracle, and fixed-seed
 * adverse spans. Hosted harness only; no crypto or device validation. */
#include "rafpgp_packet.h"
#include <stdio.h>

#define CHECK(x) do { if (!(x)) { \
    (void)fprintf(stderr, "ADVERSARIAL FAIL line %d: %s\n", __LINE__, #x); \
    return 1; \
} } while (0)

static unsigned int next_random(unsigned int *seed)
{
    unsigned int n = *seed;
    n ^= n << 13;
    n ^= n >> 17;
    n ^= n << 5;
    *seed = n;
    return n;
}

static int check_fixed_octets(void)
{
    unsigned int tag;
    unsigned int code;
    for (tag = 0u; tag < 64u; ++tag) {
        for (code = 0u; code < 256u; ++code) {
            const unsigned char packet[6] = {
                (unsigned char)(0xc0u | tag), (unsigned char)code,
                0u, 0u, 0u, 0u
            };
            struct rafpgp_header h = {77u, 77u, 77u};
            struct rafpgp_header full = {77u, 77u, 77u};
            size_t used = 777u;
            enum rafpgp_status status = rafpgp_parse_header(packet, sizeof packet, &h);
            enum rafpgp_status entire;
            unsigned long long expected_body = 0u;
            size_t expected_header = 0u;
            if (tag == 0u || (code >= 224u && code < 255u)) {
                CHECK(status == (tag == 0u ? RAFPGP_MALFORMED : RAFPGP_UNSUPPORTED));
                CHECK(h.tag == 77u && h.header_octets == 77u && h.body_octets == 77u);
                entire = rafpgp_require_complete(packet, sizeof packet, &full, &used);
                CHECK(entire == status);
                CHECK(used == 777u && full.tag == 77u);
                continue;
            }
            if (code < 192u) {
                expected_header = 2u;
                expected_body = (unsigned long long)code;
            } else if (code < 224u) {
                /* The last length byte is 0 in this fixed oracle vector. */
                expected_header = 3u;
                expected_body = (unsigned long long)(192u + 256u * (code - 192u));
            } else {
                expected_header = 6u;
                expected_body = 0u;
            }
            CHECK(status == RAFPGP_OK);
            CHECK(h.tag == tag && h.header_octets == expected_header);
            CHECK(h.body_octets == expected_body);
            entire = rafpgp_require_complete(packet, sizeof packet, &full, &used);
            if (expected_body <= (unsigned long long)(sizeof packet - expected_header)) {
                CHECK(entire == RAFPGP_OK);
                CHECK(used == expected_header + (size_t)expected_body);
                CHECK(full.tag == h.tag && full.header_octets == h.header_octets);
                CHECK(full.body_octets == h.body_octets);
            } else {
                CHECK(entire == RAFPGP_NEED_MORE);
                CHECK(used == 777u && full.tag == 77u);
            }
        }
    }
    return 0;
}

static int check_truncated(void)
{
    static const unsigned char samples[][6] = {
        {0xcbu, 0u, 0u, 0u, 0u, 0u},
        {0xcbu, 191u, 0u, 0u, 0u, 0u},
        {0xcbu, 192u, 0u, 0u, 0u, 0u},
        {0xcbu, 223u, 255u, 0u, 0u, 0u},
        {0xcbu, 224u, 0u, 0u, 0u, 0u},
        {0xcbu, 254u, 0u, 0u, 0u, 0u},
        {0xcbu, 255u, 255u, 255u, 255u, 255u}
    };
    size_t i;
    size_t n;
    for (i = 0u; i < sizeof samples / sizeof samples[0]; ++i) {
        for (n = 0u; n <= sizeof samples[0]; ++n) {
            struct rafpgp_header h = {99u, 99u, 99u};
            size_t consumed = 999u;
            enum rafpgp_status s = rafpgp_require_complete(samples[i], n, &h, &consumed);
            if (s == RAFPGP_OK) {
                CHECK(consumed <= n && h.header_octets <= consumed);
            } else {
                CHECK(h.tag == 99u && h.header_octets == 99u && h.body_octets == 99u);
                CHECK(consumed == 999u);
            }
        }
    }
    return 0;
}

static int check_random_spans(void)
{
    unsigned int seed = 0x71e1d319u;
    unsigned int iteration;
    for (iteration = 0u; iteration < 131072u; ++iteration) {
        unsigned char packet[16];
        size_t n = (size_t)(next_random(&seed) % 17u);
        size_t i;
        struct rafpgp_header parsed = {88u, 88u, 88u};
        struct rafpgp_header complete = {88u, 88u, 88u};
        size_t used = 888u;
        enum rafpgp_status s;
        enum rafpgp_status r;
        for (i = 0u; i < sizeof packet; ++i)
            packet[i] = (unsigned char)next_random(&seed);
        s = rafpgp_parse_header(packet, n, &parsed);
        r = rafpgp_require_complete(packet, n, &complete, &used);
        if (s == RAFPGP_OK) {
            CHECK(parsed.header_octets >= 2u && parsed.header_octets <= 6u);
            CHECK(parsed.header_octets <= n && parsed.tag != 0u);
            CHECK(parsed.body_octets <= 4294967295ULL);
            if (r == RAFPGP_OK) {
                CHECK(complete.tag == parsed.tag && used <= n);
                CHECK(complete.body_octets == parsed.body_octets);
                CHECK(used == complete.header_octets + (size_t)complete.body_octets);
            } else {
                CHECK(r == RAFPGP_NEED_MORE);
                CHECK(used == 888u && complete.tag == 88u);
            }
        } else {
            CHECK(r == s);
            CHECK(used == 888u && complete.tag == 88u);
            CHECK(parsed.tag == 88u && parsed.header_octets == 88u);
        }
    }
    return 0;
}

int main(void)
{
    CHECK(check_fixed_octets() == 0);
    CHECK(check_truncated() == 0);
    CHECK(check_random_spans() == 0);
    (void)puts("PASS_ADVERSARIAL: 16384 header cases + truncations + 131072 deterministic spans");
    return 0;
}
