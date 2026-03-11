/*
    FatFs library wrapper.
*/


#ifndef SD_CARD_H
#define SD_CARD_H


#include <stdbool.h>
#include <stdint.h>

#include "ff.h"


#define SD_CARD_FILE_READ		(0x01u)
#define SD_CARD_FILE_WRITE		(0x02u)
#define SD_CARD_FILE_CREATE		(0x04u)
#define SD_CARD_FILE_TRUNCATE	(0x08u)
#define SD_CARD_FILE_APPEND		(0x10u)


typedef enum
{
	SD_CARD_STATUS_OK = 0,
	SD_CARD_STATUS_ERROR,
	SD_CARD_STATUS_NOT_READY,
	SD_CARD_STATUS_NO_MEDIA,
	SD_CARD_STATUS_NOT_FOUND,
	SD_CARD_STATUS_INVALID_ARG,
	SD_CARD_STATUS_IO
} sd_card_status_t;

typedef FIL sd_card_file_t;


sd_card_status_t sd_card_init(void);
sd_card_status_t sd_card_deinit(void);
bool sd_card_is_ready(void);

sd_card_status_t sd_card_file_open(sd_card_file_t *file, const char *path, uint32_t flags);
sd_card_status_t sd_card_file_close(sd_card_file_t *file);
sd_card_status_t sd_card_file_read(sd_card_file_t *file, void *buf, uint32_t len, uint32_t *out_read);
sd_card_status_t sd_card_file_write(sd_card_file_t *file, const void *buf, uint32_t len, uint32_t *out_written);
sd_card_status_t sd_card_file_seek(sd_card_file_t *file, uint32_t offset);
sd_card_status_t sd_card_file_tell(sd_card_file_t *file, uint32_t *out_pos);
sd_card_status_t sd_card_file_size(sd_card_file_t *file, uint32_t *out_size);
sd_card_status_t sd_card_file_sync(sd_card_file_t *file);


#endif  // SD_CARD_H
