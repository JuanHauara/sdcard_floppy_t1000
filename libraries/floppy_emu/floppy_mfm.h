#ifndef FLOPPY_MFM_H
#define FLOPPY_MFM_H


#include <stddef.h>
#include <stdint.h>


#define FLOPPY_MFM_SYNC_WORD_A1    (0x4489u)


uint16_t floppy_mfm_encode_byte(uint8_t value);
void floppy_mfm_encode_bytes(const uint8_t *in, uint16_t *out, size_t len);


#endif  // FLOPPY_MFM_H
