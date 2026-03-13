#include "floppy_emu.h"

#include <string.h>

#include "floppy_crc.h"
#include "floppy_mfm.h"


#define FLOPPY_EMU_SIZE_720K     (737280u)
#define FLOPPY_EMU_SIZE_1440K    (1474560u)
#define FLOPPY_EMU_MFM_GAP_SYNC_BYTES    (12u)
#define FLOPPY_EMU_MFM_GAP_1_BYTES       (50u)
#define FLOPPY_EMU_MFM_GAP_2_BYTES       (22u)
#define FLOPPY_EMU_MFM_GAP_4A_BYTES      (80u)
#define FLOPPY_EMU_MFM_SECTOR_SIZE_CODE  (2u)


typedef struct
{
    uint16_t rpm;
    uint16_t data_rate_kbps;
    uint8_t gap_3_bytes;
} floppy_emu_track_format_t;

typedef struct
{
    uint16_t *words;
    uint32_t capacity;
    uint32_t count;
} floppy_emu_track_writer_t;


static floppy_emu_status_t floppy_emu_map_fixed_image_status(fixed_image_status_t status)
{
    switch (status)
    {
        case FIXED_IMAGE_STATUS_OK:
            return FLOPPY_EMU_STATUS_OK;

        case FIXED_IMAGE_STATUS_INVALID_ARG:
            return FLOPPY_EMU_STATUS_INVALID_ARG;

        case FIXED_IMAGE_STATUS_NOT_READY:
            return FLOPPY_EMU_STATUS_NOT_READY;

        case FIXED_IMAGE_STATUS_NOT_FOUND:
            return FLOPPY_EMU_STATUS_IMAGE_NOT_FOUND;

        case FIXED_IMAGE_STATUS_IO:
        case FIXED_IMAGE_STATUS_STORAGE_ERROR:
            return FLOPPY_EMU_STATUS_IMAGE_IO;

        case FIXED_IMAGE_STATUS_NOT_OPEN:
        default:
            return FLOPPY_EMU_STATUS_NOT_MOUNTED;
    }
}

static bool floppy_emu_detect_geometry(uint32_t size_bytes, floppy_emu_geometry_t *geometry)
{
    if (geometry == NULL)
    {
        return false;
    }

    memset(geometry, 0, sizeof(*geometry));

    switch (size_bytes)
    {
        case FLOPPY_EMU_SIZE_720K:
            geometry->cylinders = 80u;
            geometry->heads = 2u;
            geometry->sectors_per_track = 9u;
            geometry->bytes_per_sector = FLOPPY_EMU_SECTOR_SIZE;
            return true;

        case FLOPPY_EMU_SIZE_1440K:
            geometry->cylinders = 80u;
            geometry->heads = 2u;
            geometry->sectors_per_track = 18u;
            geometry->bytes_per_sector = FLOPPY_EMU_SECTOR_SIZE;
            return true;

        default:
            return false;
    }
}

static bool floppy_emu_get_track_format(const floppy_emu_t *emu,
                                        floppy_emu_track_format_t *format)
{
    if ((emu == NULL) || (format == NULL) || !emu->is_ready)
    {
        return false;
    }

    memset(format, 0, sizeof(*format));

    switch (emu->geometry.sectors_per_track)
    {
        case 9u:
            format->rpm = 300u;
            format->data_rate_kbps = 250u;
            format->gap_3_bytes = 84u;
            return true;

        case 18u:
            format->rpm = 300u;
            format->data_rate_kbps = 500u;
            format->gap_3_bytes = 108u;
            return true;

        default:
            return false;
    }
}

static uint32_t floppy_emu_get_track_word_count(const floppy_emu_track_format_t *format)
{
    if ((format == NULL) || (format->rpm == 0u))
    {
        return 0u;
    }

    return ((uint32_t)format->data_rate_kbps * 400u * 300u)
         / ((uint32_t)format->rpm * 16u);
}

static floppy_emu_status_t floppy_emu_track_emit_word(floppy_emu_track_writer_t *writer,
                                                      uint16_t word)
{
    if ((writer == NULL) || (writer->words == NULL))
    {
        return FLOPPY_EMU_STATUS_INVALID_ARG;
    }

    if (writer->count >= writer->capacity)
    {
        return FLOPPY_EMU_STATUS_BUFFER_TOO_SMALL;
    }

    writer->words[writer->count] = word;
    writer->count++;

    return FLOPPY_EMU_STATUS_OK;
}

static floppy_emu_status_t floppy_emu_track_emit_byte(floppy_emu_track_writer_t *writer,
                                                      uint8_t value)
{
    return floppy_emu_track_emit_word(writer, floppy_mfm_encode_byte(value));
}

static floppy_emu_status_t floppy_emu_track_emit_fill(floppy_emu_track_writer_t *writer,
                                                      uint8_t value,
                                                      uint32_t count)
{
    floppy_emu_status_t status = FLOPPY_EMU_STATUS_OK;
    uint32_t index = 0u;

    while ((index < count) && (status == FLOPPY_EMU_STATUS_OK))
    {
        status = floppy_emu_track_emit_byte(writer, value);
        index++;
    }

    return status;
}

static floppy_emu_status_t floppy_emu_track_emit_sync(floppy_emu_track_writer_t *writer,
                                                      uint32_t count)
{
    floppy_emu_status_t status = FLOPPY_EMU_STATUS_OK;
    uint32_t index = 0u;

    while ((index < count) && (status == FLOPPY_EMU_STATUS_OK))
    {
        status = floppy_emu_track_emit_word(writer, FLOPPY_MFM_SYNC_WORD_A1);
        index++;
    }

    return status;
}

static floppy_emu_status_t floppy_emu_track_emit_crc(floppy_emu_track_writer_t *writer,
                                                     uint16_t crc)
{
    floppy_emu_status_t status;

    status = floppy_emu_track_emit_byte(writer, (uint8_t)(crc >> 8));
    if (status != FLOPPY_EMU_STATUS_OK)
    {
        return status;
    }

    return floppy_emu_track_emit_byte(writer, (uint8_t)crc);
}

static floppy_emu_status_t floppy_emu_track_emit_idam(floppy_emu_track_writer_t *writer,
                                                      uint16_t cylinder,
                                                      uint8_t head,
                                                      uint8_t sector_id)
{
    const uint8_t header[] =
    {
        0xa1u, 0xa1u, 0xa1u, 0xfeu,
        (uint8_t)cylinder, head, sector_id, FLOPPY_EMU_MFM_SECTOR_SIZE_CODE
    };
    floppy_emu_status_t status;
    uint16_t crc;
    uint32_t index = 3u;

    status = floppy_emu_track_emit_fill(writer, 0x00u, FLOPPY_EMU_MFM_GAP_SYNC_BYTES);
    if (status != FLOPPY_EMU_STATUS_OK)
    {
        return status;
    }

    status = floppy_emu_track_emit_sync(writer, 3u);
    if (status != FLOPPY_EMU_STATUS_OK)
    {
        return status;
    }

    while ((index < sizeof(header)) && (status == FLOPPY_EMU_STATUS_OK))
    {
        status = floppy_emu_track_emit_byte(writer, header[index]);
        index++;
    }

    if (status != FLOPPY_EMU_STATUS_OK)
    {
        return status;
    }

    crc = floppy_crc16_ccitt(header, sizeof(header), 0xffffu);
    status = floppy_emu_track_emit_crc(writer, crc);
    if (status != FLOPPY_EMU_STATUS_OK)
    {
        return status;
    }

    return floppy_emu_track_emit_fill(writer, 0x4eu, FLOPPY_EMU_MFM_GAP_2_BYTES);
}

static floppy_emu_status_t floppy_emu_track_emit_dam(floppy_emu_track_writer_t *writer,
                                                     const uint8_t *sector_data,
                                                     uint32_t sector_len,
                                                     uint8_t gap_3_bytes)
{
    const uint8_t header[] = { 0xa1u, 0xa1u, 0xa1u, 0xfbu };
    floppy_emu_status_t status;
    uint16_t crc;
    uint32_t index = 0u;

    if (sector_data == NULL)
    {
        return FLOPPY_EMU_STATUS_INVALID_ARG;
    }

    status = floppy_emu_track_emit_fill(writer, 0x00u, FLOPPY_EMU_MFM_GAP_SYNC_BYTES);
    if (status != FLOPPY_EMU_STATUS_OK)
    {
        return status;
    }

    status = floppy_emu_track_emit_sync(writer, 3u);
    if (status != FLOPPY_EMU_STATUS_OK)
    {
        return status;
    }

    status = floppy_emu_track_emit_byte(writer, 0xfbu);
    if (status != FLOPPY_EMU_STATUS_OK)
    {
        return status;
    }

    crc = floppy_crc16_ccitt(header, sizeof(header), 0xffffu);

    while ((index < sector_len) && (status == FLOPPY_EMU_STATUS_OK))
    {
        status = floppy_emu_track_emit_byte(writer, sector_data[index]);
        index++;
    }

    if (status != FLOPPY_EMU_STATUS_OK)
    {
        return status;
    }

    crc = floppy_crc16_ccitt(sector_data, sector_len, crc);
    status = floppy_emu_track_emit_crc(writer, crc);
    if (status != FLOPPY_EMU_STATUS_OK)
    {
        return status;
    }

    return floppy_emu_track_emit_fill(writer, 0x4eu, gap_3_bytes);
}

static floppy_emu_status_t floppy_emu_chs_to_lba(const floppy_emu_t *emu,
                                                 uint16_t cylinder,
                                                 uint8_t head,
                                                 uint8_t sector_id,
                                                 uint32_t *out_lba)
{
    uint32_t lba;

    if ((emu == NULL) || (out_lba == NULL))
    {
        return FLOPPY_EMU_STATUS_INVALID_ARG;
    }

    if (!emu->is_ready)
    {
        return FLOPPY_EMU_STATUS_NOT_MOUNTED;
    }

    if ((head >= emu->geometry.heads)
        || (sector_id == 0u)
        || (sector_id > emu->geometry.sectors_per_track)
        || (cylinder >= emu->geometry.cylinders))
    {
        return FLOPPY_EMU_STATUS_INVALID_ARG;
    }

    lba = ((uint32_t)cylinder * (uint32_t)emu->geometry.heads) + (uint32_t)head;
    lba *= (uint32_t)emu->geometry.sectors_per_track;
    lba += (uint32_t)(sector_id - 1u);

    *out_lba = lba;

    return FLOPPY_EMU_STATUS_OK;
}

floppy_emu_status_t floppy_emu_mount_fixed_image(floppy_emu_t *emu)
{
    fixed_image_status_t image_status;

    if (emu == NULL)
    {
        return FLOPPY_EMU_STATUS_INVALID_ARG;
    }

    memset(emu, 0, sizeof(*emu));

    image_status = fixed_image_open(&emu->image);
    if (image_status != FIXED_IMAGE_STATUS_OK)
    {
        return floppy_emu_map_fixed_image_status(image_status);
    }

    image_status = fixed_image_get_size(&emu->image, &emu->size_bytes);
    if (image_status != FIXED_IMAGE_STATUS_OK)
    {
        (void)fixed_image_close(&emu->image);

        return floppy_emu_map_fixed_image_status(image_status);
    }

    if (!floppy_emu_detect_geometry(emu->size_bytes, &emu->geometry))
    {
        (void)fixed_image_close(&emu->image);

        return FLOPPY_EMU_STATUS_UNSUPPORTED_IMAGE;
    }

    emu->is_ready = true;
    emu->write_protected = true;

    return FLOPPY_EMU_STATUS_OK;
}

void floppy_emu_unmount(floppy_emu_t *emu)
{
    if (emu == NULL)
    {
        return;
    }

    if (emu->image.is_open)
    {
        (void)fixed_image_close(&emu->image);
    }

    memset(emu, 0, sizeof(*emu));
}

bool floppy_emu_is_ready(const floppy_emu_t *emu)
{
    return ((emu != NULL) && emu->is_ready);
}

const char *floppy_emu_get_path(const floppy_emu_t *emu)
{
    if (emu == NULL)
    {
        return "";
    }

    return fixed_image_get_path(&emu->image);
}

const floppy_emu_geometry_t *floppy_emu_get_geometry(const floppy_emu_t *emu)
{
    if (emu == NULL)
    {
        return NULL;
    }

    return &emu->geometry;
}

uint32_t floppy_emu_get_sector_count(const floppy_emu_t *emu)
{
    if ((emu == NULL) || !emu->is_ready || (emu->geometry.bytes_per_sector == 0u))
    {
        return 0u;
    }

    return emu->size_bytes / (uint32_t)emu->geometry.bytes_per_sector;
}

floppy_emu_status_t floppy_emu_read_lba_sector(floppy_emu_t *emu,
                                               uint32_t lba,
                                               uint8_t *buf,
                                               uint32_t buf_len)
{
    fixed_image_status_t image_status;
    uint32_t bytes_read = 0u;
    uint32_t sector_count = 0u;
    uint32_t offset = 0u;

    if ((emu == NULL) || (buf == NULL))
    {
        return FLOPPY_EMU_STATUS_INVALID_ARG;
    }

    if (!emu->is_ready)
    {
        return FLOPPY_EMU_STATUS_NOT_MOUNTED;
    }

    if (buf_len < FLOPPY_EMU_SECTOR_SIZE)
    {
        return FLOPPY_EMU_STATUS_INVALID_ARG;
    }

    sector_count = floppy_emu_get_sector_count(emu);
    if (lba >= sector_count)
    {
        return FLOPPY_EMU_STATUS_INVALID_ARG;
    }

    offset = lba * FLOPPY_EMU_SECTOR_SIZE;
    image_status = fixed_image_read(&emu->image,
                                    offset,
                                    buf,
                                    FLOPPY_EMU_SECTOR_SIZE,
                                    &bytes_read);
    if (image_status != FIXED_IMAGE_STATUS_OK)
    {
        return floppy_emu_map_fixed_image_status(image_status);
    }

    if (bytes_read != FLOPPY_EMU_SECTOR_SIZE)
    {
        return FLOPPY_EMU_STATUS_IMAGE_IO;
    }

    return FLOPPY_EMU_STATUS_OK;
}

floppy_emu_status_t floppy_emu_read_chs_sector(floppy_emu_t *emu,
                                               uint16_t cylinder,
                                               uint8_t head,
                                               uint8_t sector_id,
                                               uint8_t *buf,
                                               uint32_t buf_len)
{
    uint32_t lba = 0u;
    floppy_emu_status_t status;

    status = floppy_emu_chs_to_lba(emu, cylinder, head, sector_id, &lba);
    if (status != FLOPPY_EMU_STATUS_OK)
    {
        return status;
    }

    return floppy_emu_read_lba_sector(emu, lba, buf, buf_len);
}

floppy_emu_status_t floppy_emu_build_mfm_track(floppy_emu_t *emu,
                                               uint16_t cylinder,
                                               uint8_t head,
                                               uint16_t *out_words,
                                               uint32_t max_words,
                                               floppy_emu_track_info_t *out_info)
{
    floppy_emu_track_format_t format;
    floppy_emu_track_writer_t writer;
    floppy_emu_status_t status = FLOPPY_EMU_STATUS_OK;
    uint8_t sector_data[FLOPPY_EMU_SECTOR_SIZE];
    uint32_t track_word_count;
    uint32_t pre_index_gap_words;
    uint8_t sector_id = 0u;

    if ((emu == NULL) || (out_words == NULL))
    {
        return FLOPPY_EMU_STATUS_INVALID_ARG;
    }

    if (!emu->is_ready)
    {
        return FLOPPY_EMU_STATUS_NOT_MOUNTED;
    }

    if ((cylinder >= emu->geometry.cylinders) || (head >= emu->geometry.heads))
    {
        return FLOPPY_EMU_STATUS_INVALID_ARG;
    }

    if (!floppy_emu_get_track_format(emu, &format))
    {
        return FLOPPY_EMU_STATUS_UNSUPPORTED_IMAGE;
    }

    track_word_count = floppy_emu_get_track_word_count(&format);
    if (max_words < track_word_count)
    {
        return FLOPPY_EMU_STATUS_BUFFER_TOO_SMALL;
    }

    writer.words = out_words;
    writer.capacity = max_words;
    writer.count = 0u;

    status = floppy_emu_track_emit_fill(&writer, 0x4eu, FLOPPY_EMU_MFM_GAP_4A_BYTES);
    if (status != FLOPPY_EMU_STATUS_OK)
    {
        return status;
    }

    status = floppy_emu_track_emit_fill(&writer, 0x00u, FLOPPY_EMU_MFM_GAP_SYNC_BYTES);
    if (status != FLOPPY_EMU_STATUS_OK)
    {
        return status;
    }

    status = floppy_emu_track_emit_sync(&writer, 3u);
    if (status != FLOPPY_EMU_STATUS_OK)
    {
        return status;
    }

    status = floppy_emu_track_emit_byte(&writer, 0xfcu);
    if (status != FLOPPY_EMU_STATUS_OK)
    {
        return status;
    }

    status = floppy_emu_track_emit_fill(&writer, 0x4eu, FLOPPY_EMU_MFM_GAP_1_BYTES);
    if (status != FLOPPY_EMU_STATUS_OK)
    {
        return status;
    }

    for (sector_id = 1u;
         sector_id <= emu->geometry.sectors_per_track;
         sector_id++)
    {
        status = floppy_emu_read_chs_sector(emu,
                                            cylinder,
                                            head,
                                            sector_id,
                                            sector_data,
                                            sizeof(sector_data));
        if (status != FLOPPY_EMU_STATUS_OK)
        {
            return status;
        }

        status = floppy_emu_track_emit_idam(&writer, cylinder, head, sector_id);
        if (status != FLOPPY_EMU_STATUS_OK)
        {
            return status;
        }

        status = floppy_emu_track_emit_dam(&writer,
                                           sector_data,
                                           sizeof(sector_data),
                                           format.gap_3_bytes);
        if (status != FLOPPY_EMU_STATUS_OK)
        {
            return status;
        }
    }

    if (writer.count > track_word_count)
    {
        return FLOPPY_EMU_STATUS_UNSUPPORTED_IMAGE;
    }

    pre_index_gap_words = track_word_count - writer.count;
    status = floppy_emu_track_emit_fill(&writer, 0x4eu, pre_index_gap_words);
    if (status != FLOPPY_EMU_STATUS_OK)
    {
        return status;
    }

    if (out_info != NULL)
    {
        memset(out_info, 0, sizeof(*out_info));
        out_info->rpm = format.rpm;
        out_info->data_rate_kbps = format.data_rate_kbps;
        out_info->track_word_count = track_word_count;
        out_info->gap_4a_words = FLOPPY_EMU_MFM_GAP_4A_BYTES;
        out_info->gap_3_words = format.gap_3_bytes;
        out_info->pre_index_gap_words = pre_index_gap_words;
    }

    return FLOPPY_EMU_STATUS_OK;
}

const char *floppy_emu_status_str(floppy_emu_status_t status)
{
    switch (status)
    {
        case FLOPPY_EMU_STATUS_OK:
            return "ok";

        case FLOPPY_EMU_STATUS_INVALID_ARG:
            return "invalid_arg";

        case FLOPPY_EMU_STATUS_NOT_READY:
            return "not_ready";

        case FLOPPY_EMU_STATUS_IMAGE_NOT_FOUND:
            return "image_not_found";

        case FLOPPY_EMU_STATUS_IMAGE_IO:
            return "image_io";

        case FLOPPY_EMU_STATUS_BUFFER_TOO_SMALL:
            return "buffer_too_small";

        case FLOPPY_EMU_STATUS_UNSUPPORTED_IMAGE:
            return "unsupported_image";

        case FLOPPY_EMU_STATUS_NOT_MOUNTED:
            return "not_mounted";

        default:
            return "unknown";
    }
}
