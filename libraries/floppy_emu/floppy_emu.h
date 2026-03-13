#ifndef FLOPPY_EMU_H
#define FLOPPY_EMU_H


#include <stdbool.h>
#include <stdint.h>

#include "storage/fixed_image.h"


#define FLOPPY_EMU_SECTOR_SIZE    (512u)
#define FLOPPY_EMU_TRACK_WORDS_720K     (6250u)
#define FLOPPY_EMU_TRACK_WORDS_1440K    (12500u)
#define FLOPPY_EMU_MAX_TRACK_WORDS      FLOPPY_EMU_TRACK_WORDS_1440K


typedef enum
{
    FLOPPY_EMU_STATUS_OK = 0,
    FLOPPY_EMU_STATUS_INVALID_ARG,
    FLOPPY_EMU_STATUS_NOT_READY,
    FLOPPY_EMU_STATUS_IMAGE_NOT_FOUND,
    FLOPPY_EMU_STATUS_IMAGE_IO,
    FLOPPY_EMU_STATUS_BUFFER_TOO_SMALL,
    FLOPPY_EMU_STATUS_UNSUPPORTED_IMAGE,
    FLOPPY_EMU_STATUS_NOT_MOUNTED
} floppy_emu_status_t;

typedef struct
{
    uint16_t cylinders;
    uint8_t heads;
    uint8_t sectors_per_track;
    uint16_t bytes_per_sector;
} floppy_emu_geometry_t;

typedef struct
{
    uint16_t rpm;
    uint16_t data_rate_kbps;
    uint32_t track_word_count;
    uint32_t gap_4a_words;
    uint32_t gap_3_words;
    uint32_t pre_index_gap_words;
} floppy_emu_track_info_t;

typedef struct
{
    fixed_image_t image;
    floppy_emu_geometry_t geometry;
    uint32_t size_bytes;
    bool is_ready;
    bool write_protected;
} floppy_emu_t;


floppy_emu_status_t floppy_emu_mount_fixed_image(floppy_emu_t *emu);
void floppy_emu_unmount(floppy_emu_t *emu);
bool floppy_emu_is_ready(const floppy_emu_t *emu);
const char *floppy_emu_get_path(const floppy_emu_t *emu);
const floppy_emu_geometry_t *floppy_emu_get_geometry(const floppy_emu_t *emu);
uint32_t floppy_emu_get_sector_count(const floppy_emu_t *emu);
floppy_emu_status_t floppy_emu_read_lba_sector(floppy_emu_t *emu,
                                               uint32_t lba,
                                               uint8_t *buf,
                                               uint32_t buf_len);
floppy_emu_status_t floppy_emu_read_chs_sector(floppy_emu_t *emu,
                                               uint16_t cylinder,
                                               uint8_t head,
                                               uint8_t sector_id,
                                               uint8_t *buf,
                                               uint32_t buf_len);
floppy_emu_status_t floppy_emu_build_mfm_track(floppy_emu_t *emu,
                                               uint16_t cylinder,
                                               uint8_t head,
                                               uint16_t *out_words,
                                               uint32_t max_words,
                                               floppy_emu_track_info_t *out_info);
const char *floppy_emu_status_str(floppy_emu_status_t status);


#endif  // FLOPPY_EMU_H
