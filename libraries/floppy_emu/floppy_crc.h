#ifndef FLOPPY_CRC_H
#define FLOPPY_CRC_H


#include <stddef.h>
#include <stdint.h>


uint16_t floppy_crc16_ccitt(const void *buf, size_t len, uint16_t crc);


#endif  // FLOPPY_CRC_H
