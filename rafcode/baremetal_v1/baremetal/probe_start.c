/* SPDX-License-Identifier: MIT
 * Link-only synthetic entrypoint. NOT a board bootstrap or firmware.
 */
#include "raf_octets.h"
static volatile unsigned int raf_probe_signal;
void _start(void) {
    unsigned char bytes[4];
    unsigned int value = 0u;
    unsigned int success = 0u;
    if (raf_octets_write_be32(bytes, 4ul, 0x1234ABCDu) == RAF_OCTETS_OK &&
        raf_octets_read_be32(bytes, 4ul, &value) == RAF_OCTETS_OK &&
        value == 0x1234ABCDu) {
        success = 1u;
    }
    for (;;) raf_probe_signal = success;
}
