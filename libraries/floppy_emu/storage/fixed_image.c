#include "fixed_image.h"

#include <stdio.h>
#include <string.h>

#include "fatfs.h"


#define FIXED_IMAGE_FILENAME    "disk.img"
#define FALLBACK_DRIVE_PATH     "0:/"


static fixed_image_status_t fixed_image_map_sd_status(sd_card_status_t status)
{
    switch (status)
    {
        case SD_CARD_STATUS_OK:
            return FIXED_IMAGE_STATUS_OK;

        case SD_CARD_STATUS_INVALID_ARG:
            return FIXED_IMAGE_STATUS_INVALID_ARG;

        case SD_CARD_STATUS_NOT_READY:
        case SD_CARD_STATUS_NO_MEDIA:
            return FIXED_IMAGE_STATUS_NOT_READY;

        case SD_CARD_STATUS_NOT_FOUND:
            return FIXED_IMAGE_STATUS_NOT_FOUND;

        case SD_CARD_STATUS_IO:
            return FIXED_IMAGE_STATUS_IO;

        case SD_CARD_STATUS_ERROR:
        default:
            return FIXED_IMAGE_STATUS_STORAGE_ERROR;
    }
}

static void fixed_image_build_path(char *path, uint32_t path_len)
{
    const char *drive_path = USERPath;
    uint32_t drive_len = 0u;

    if ((path == NULL) || (path_len == 0u))
    {
        return;
    }

    if ((drive_path == NULL) || (drive_path[0] == '\0'))
    {
        drive_path = FALLBACK_DRIVE_PATH;
    }

    drive_len = (uint32_t)strlen(drive_path);
    if ((drive_len > 0u) && (drive_path[drive_len - 1u] == '/'))
    {
        (void)snprintf(path, path_len, "%s%s", drive_path, FIXED_IMAGE_FILENAME);
    }
    else
    {
        (void)snprintf(path, path_len, "%s/%s", drive_path, FIXED_IMAGE_FILENAME);
    }
}

fixed_image_status_t fixed_image_open(fixed_image_t *image)
{
    sd_card_status_t sd_status;

    if (image == NULL)
    {
        return FIXED_IMAGE_STATUS_INVALID_ARG;
    }

    memset(image, 0, sizeof(*image));

    if (!sd_card_is_ready())
    {
        sd_status = sd_card_init();
        if (sd_status != SD_CARD_STATUS_OK)
        {
            return fixed_image_map_sd_status(sd_status);
        }
    }

    fixed_image_build_path(image->path, sizeof(image->path));

    sd_status = sd_card_file_open(&image->file, image->path, SD_CARD_FILE_READ);
    if (sd_status != SD_CARD_STATUS_OK)
    {
        return fixed_image_map_sd_status(sd_status);
    }

    sd_status = sd_card_file_size(&image->file, &image->size_bytes);
    if (sd_status != SD_CARD_STATUS_OK)
    {
        (void)sd_card_file_close(&image->file);
        memset(image, 0, sizeof(*image));

        return fixed_image_map_sd_status(sd_status);
    }

    image->is_open = true;

    return FIXED_IMAGE_STATUS_OK;
}

fixed_image_status_t fixed_image_close(fixed_image_t *image)
{
    sd_card_status_t sd_status;

    if (image == NULL)
    {
        return FIXED_IMAGE_STATUS_INVALID_ARG;
    }

    if (!image->is_open)
    {
        return FIXED_IMAGE_STATUS_OK;
    }

    sd_status = sd_card_file_close(&image->file);
    image->is_open = false;
    image->size_bytes = 0u;
    image->path[0] = '\0';

    return fixed_image_map_sd_status(sd_status);
}

bool fixed_image_is_open(const fixed_image_t *image)
{
    return ((image != NULL) && image->is_open);
}

const char *fixed_image_get_path(const fixed_image_t *image)
{
    if (image == NULL)
    {
        return "";
    }

    return image->path;
}

fixed_image_status_t fixed_image_get_size(const fixed_image_t *image, uint32_t *out_size)
{
    if ((image == NULL) || (out_size == NULL))
    {
        return FIXED_IMAGE_STATUS_INVALID_ARG;
    }

    if (!image->is_open)
    {
        return FIXED_IMAGE_STATUS_NOT_OPEN;
    }

    *out_size = image->size_bytes;

    return FIXED_IMAGE_STATUS_OK;
}

fixed_image_status_t fixed_image_read(fixed_image_t *image,
                                      uint32_t offset,
                                      void *buf,
                                      uint32_t len,
                                      uint32_t *out_read)
{
    sd_card_status_t sd_status;

    if ((image == NULL) || (buf == NULL) || (out_read == NULL))
    {
        return FIXED_IMAGE_STATUS_INVALID_ARG;
    }

    if (!image->is_open)
    {
        return FIXED_IMAGE_STATUS_NOT_OPEN;
    }

    sd_status = sd_card_file_seek(&image->file, offset);
    if (sd_status != SD_CARD_STATUS_OK)
    {
        return fixed_image_map_sd_status(sd_status);
    }

    sd_status = sd_card_file_read(&image->file, buf, len, out_read);

    return fixed_image_map_sd_status(sd_status);
}

const char *fixed_image_status_str(fixed_image_status_t status)
{
    switch (status)
    {
        case FIXED_IMAGE_STATUS_OK:
            return "ok";

        case FIXED_IMAGE_STATUS_INVALID_ARG:
            return "invalid_arg";

        case FIXED_IMAGE_STATUS_STORAGE_ERROR:
            return "storage_error";

        case FIXED_IMAGE_STATUS_NOT_READY:
            return "not_ready";

        case FIXED_IMAGE_STATUS_NOT_FOUND:
            return "not_found";

        case FIXED_IMAGE_STATUS_IO:
            return "io";

        case FIXED_IMAGE_STATUS_NOT_OPEN:
            return "not_open";

        default:
            return "unknown";
    }
}
