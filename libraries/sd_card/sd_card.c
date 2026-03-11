#include "sd_card.h"


static FATFS s_fatfs;
static bool s_is_mounted = false;


static sd_card_status_t sd_card_map_fresult(FRESULT res)
{
	switch (res)
	{
		case FR_OK:
			return SD_CARD_STATUS_OK;

		case FR_NOT_READY:
			return SD_CARD_STATUS_NOT_READY;

		case FR_NO_FILE:
		case FR_NO_PATH:
			return SD_CARD_STATUS_NOT_FOUND;

		case FR_INVALID_NAME:
		case FR_INVALID_PARAMETER:
			return SD_CARD_STATUS_INVALID_ARG;

		case FR_DISK_ERR:
		case FR_INT_ERR:
		case FR_NOT_ENABLED:
		case FR_NO_FILESYSTEM:
			return SD_CARD_STATUS_IO;

		case FR_WRITE_PROTECTED:
		case FR_DENIED:
		case FR_EXIST:
		case FR_TIMEOUT:
		default:
			return SD_CARD_STATUS_ERROR;
	}
}

static bool sd_card_is_flag_set(uint32_t flags, uint32_t flag)
{
	return ((flags & flag) != 0u);
}

sd_card_status_t sd_card_init(void)
{
	s_is_mounted = false;
	FRESULT res = f_mount(&s_fatfs, "", 1u);
	if (res == FR_OK)
	{
		s_is_mounted = true;
	}

	return sd_card_map_fresult(res);
}

sd_card_status_t sd_card_deinit(void)
{
	FRESULT res = f_mount(NULL, "", 1u);
	s_is_mounted = false;

	return sd_card_map_fresult(res);
}

bool sd_card_is_ready(void)
{
	return s_is_mounted;
}

sd_card_status_t sd_card_file_open(sd_card_file_t *file, const char *path, uint32_t flags)
{
	BYTE mode = 0u;
	FRESULT res = FR_INVALID_PARAMETER;

	if ( (file == NULL) || (path == NULL) )
	{
		return SD_CARD_STATUS_INVALID_ARG;
	}

	if (!s_is_mounted)
	{
		return SD_CARD_STATUS_NOT_READY;
	}

	if (sd_card_is_flag_set(flags, SD_CARD_FILE_READ))
	{
		mode |= FA_READ;
	}

	if (sd_card_is_flag_set(flags, SD_CARD_FILE_WRITE))
	{
		mode |= FA_WRITE;
	}

	if (sd_card_is_flag_set(flags, SD_CARD_FILE_CREATE))
	{
		if (sd_card_is_flag_set(flags, SD_CARD_FILE_TRUNCATE))
		{
			mode |= FA_CREATE_ALWAYS;
		}
		else
		{
			mode |= FA_OPEN_ALWAYS;
		}
	}
	else
	{
		mode |= FA_OPEN_EXISTING;
	}

	res = f_open(file, path, mode);
	if (res != FR_OK)
	{
		return sd_card_map_fresult(res);
	}

	if (sd_card_is_flag_set(flags, SD_CARD_FILE_APPEND))
	{
		res = f_lseek(file, f_size(file));
		if (res != FR_OK)
		{
			(void)f_close(file);

			return sd_card_map_fresult(res);
		}
	}

	return SD_CARD_STATUS_OK;
}

sd_card_status_t sd_card_file_close(sd_card_file_t *file)
{
	if (file == NULL)
	{
		return SD_CARD_STATUS_INVALID_ARG;
	}

	return sd_card_map_fresult(f_close(file));
}

sd_card_status_t sd_card_file_read(sd_card_file_t *file, void *buf, uint32_t len, uint32_t *out_read)
{
	UINT bytes_read = 0u;
	FRESULT res = FR_INVALID_PARAMETER;

	if ( (file == NULL) || (buf == NULL) || (out_read == NULL) )
	{
		return SD_CARD_STATUS_INVALID_ARG;
	}

	res = f_read(file, buf, (UINT)len, &bytes_read);
	*out_read = (uint32_t)bytes_read;

	return sd_card_map_fresult(res);
}

sd_card_status_t sd_card_file_write(sd_card_file_t *file,
									const void *buf,
									uint32_t len,
									uint32_t *out_written)
{
	UINT bytes_written = 0u;
	FRESULT res = FR_INVALID_PARAMETER;

	if ( (file == NULL) || (buf == NULL) || (out_written == NULL) )
	{
		return SD_CARD_STATUS_INVALID_ARG;
	}

	res = f_write(file, buf, (UINT)len, &bytes_written);
	*out_written = (uint32_t)bytes_written;

	return sd_card_map_fresult(res);
}

sd_card_status_t sd_card_file_seek(sd_card_file_t *file, uint32_t offset)
{
	if (file == NULL)
	{
		return SD_CARD_STATUS_INVALID_ARG;
	}

	return sd_card_map_fresult(f_lseek(file, (FSIZE_t)offset));
}

sd_card_status_t sd_card_file_tell(sd_card_file_t *file, uint32_t *out_pos)
{
	FSIZE_t pos = 0u;

	if ( (file == NULL) || (out_pos == NULL) )
	{
		return SD_CARD_STATUS_INVALID_ARG;
	}

	pos = f_tell(file);
	if (pos > 0xFFFFFFFFu)
	{
		return SD_CARD_STATUS_ERROR;
	}

	*out_pos = (uint32_t)pos;

	return SD_CARD_STATUS_OK;
}

sd_card_status_t sd_card_file_size(sd_card_file_t *file, uint32_t *out_size)
{
	FSIZE_t size = 0u;

	if ( (file == NULL) || (out_size == NULL) )
	{
		return SD_CARD_STATUS_INVALID_ARG;
	}

	size = f_size(file);
	if (size > 0xFFFFFFFFu)
	{
		return SD_CARD_STATUS_ERROR;
	}

	*out_size = (uint32_t)size;

	return SD_CARD_STATUS_OK;
}

sd_card_status_t sd_card_file_sync(sd_card_file_t *file)
{
	if (file == NULL)
	{
		return SD_CARD_STATUS_INVALID_ARG;
	}
	
	return sd_card_map_fresult(f_sync(file));
}
