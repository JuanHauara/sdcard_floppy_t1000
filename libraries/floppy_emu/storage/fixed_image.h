#ifndef FIXED_IMAGE_H
#define FIXED_IMAGE_H


#include <stdbool.h>
#include <stdint.h>

#include "sd_card.h"


#define FIXED_IMAGE_PATH_MAX_LEN    (32u)


typedef enum
{
    FIXED_IMAGE_STATUS_OK = 0,
    FIXED_IMAGE_STATUS_INVALID_ARG,
    FIXED_IMAGE_STATUS_STORAGE_ERROR,
    FIXED_IMAGE_STATUS_NOT_READY,
    FIXED_IMAGE_STATUS_NOT_FOUND,
    FIXED_IMAGE_STATUS_IO,
    FIXED_IMAGE_STATUS_NOT_OPEN
} fixed_image_status_t;

typedef struct
{
    sd_card_file_t file;
    uint32_t size_bytes;
    bool is_open;
    char path[FIXED_IMAGE_PATH_MAX_LEN];
} fixed_image_t;


fixed_image_status_t fixed_image_open(fixed_image_t *image);
fixed_image_status_t fixed_image_close(fixed_image_t *image);
bool fixed_image_is_open(const fixed_image_t *image);
const char *fixed_image_get_path(const fixed_image_t *image);
fixed_image_status_t fixed_image_get_size(const fixed_image_t *image, uint32_t *out_size);
fixed_image_status_t fixed_image_read(fixed_image_t *image,
                                      uint32_t offset,
                                      void *buf,
                                      uint32_t len,
                                      uint32_t *out_read);
const char *fixed_image_status_str(fixed_image_status_t status);


#endif  // FIXED_IMAGE_H
