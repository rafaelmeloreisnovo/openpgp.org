#include "raf_octets.h"
#define CHECK(x) do { if (!(x)) return __LINE__; } while (0)
int main(void) {
    unsigned char wire[6] = {9u, 9u, 9u, 9u, 7u, 7u};
    unsigned char a[4] = {0xF0u, 0x0Fu, 0xA5u, 0x5Au};
    unsigned char b[4] = {0x0Fu, 0xF0u, 0xA5u, 0x5Au};
    unsigned int decoded = 0xDEADBEEFu;
    CHECK(raf_octets_write_be32(wire, 3ul, 0x01020304u) == RAF_OCTETS_RANGE);
    CHECK(wire[0] == 9u && wire[3] == 9u);
    CHECK(raf_octets_write_be32((void *)0, 4ul, 1u) == RAF_OCTETS_NULL);
    CHECK(raf_octets_write_be32(wire, 6ul, 0x01020304u) == RAF_OCTETS_OK);
    CHECK(wire[0] == 1u && wire[1] == 2u && wire[2] == 3u && wire[3] == 4u);
    CHECK(wire[4] == 7u && wire[5] == 7u);
    CHECK(raf_octets_read_be32(wire, 3ul, &decoded) == RAF_OCTETS_RANGE);
    CHECK(decoded == 0xDEADBEEFu);
    CHECK(raf_octets_read_be32((void *)0, 4ul, &decoded) == RAF_OCTETS_NULL);
    CHECK(raf_octets_read_be32(wire, 4ul, (void *)0) == RAF_OCTETS_NULL);
    CHECK(raf_octets_read_be32(wire, 4ul, &decoded) == RAF_OCTETS_OK);
    CHECK(decoded == 0x01020304u);
    CHECK(raf_octets_xor(wire, 3ul, a, b, 4ul) == RAF_OCTETS_RANGE);
    CHECK(wire[0] == 1u && wire[3] == 4u);
    CHECK(raf_octets_xor((void *)0, 0ul, 0, 0, 0ul) == RAF_OCTETS_OK);
    CHECK(raf_octets_xor(wire, 4ul, a, b, 4ul) == RAF_OCTETS_OK);
    CHECK(wire[0] == 255u && wire[1] == 255u && wire[2] == 0u && wire[3] == 0u);
    CHECK(raf_octets_xor(a, 4ul, a, b, 4ul) == RAF_OCTETS_OK);
    CHECK(a[0] == 255u && a[1] == 255u && a[2] == 0u && a[3] == 0u);
    CHECK(raf_octets_different(wire, a, 4ul) == 0);
    CHECK(raf_octets_different(wire, b, 4ul) == 1);
    CHECK(raf_octets_different(0, b, 4ul) == -1);
    CHECK(raf_octets_different(0, 0, 0ul) == 0);
    CHECK(raf_octets_volatile_zero(wire, 4ul) == RAF_OCTETS_OK);
    CHECK(wire[0] == 0u && wire[1] == 0u && wire[2] == 0u && wire[3] == 0u);
    CHECK(wire[4] == 7u && wire[5] == 7u);
    CHECK(raf_octets_volatile_zero(0, 4ul) == RAF_OCTETS_NULL);
    CHECK(raf_octets_volatile_zero(0, 0ul) == RAF_OCTETS_OK);
    return 0;
}
