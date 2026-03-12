/*
	SPI SD-card interface used to initialize the card and perform block-level
	read/write operations. It is intended to implement the STM32CubeIDE-generated
	FATFS "User-defined" disk I/O layer (typically user_diskio.c).
*/

#ifndef SD_SPI_H
#define SD_SPI_H


#include <stdbool.h>
#include <stdint.h>


#define SD_SPI_PHYSICAL_DRIVE			(0u)
#define SD_SPI_SECTOR_SIZE				(512u)
#define SD_SPI_ERASE_BLOCK_SIZE			(1u)


typedef enum
{
	SD_SPI_OK = 0,
	SD_SPI_ERR,
	SD_SPI_TIMEOUT,
	SD_SPI_NOT_READY
} sd_spi_status_t;


sd_spi_status_t sd_spi_init(void);
bool sd_spi_is_ready(void);

sd_spi_status_t sd_spi_read_blocks(uint8_t *dst, uint32_t lba, uint32_t count);
sd_spi_status_t sd_spi_write_blocks(const uint8_t *src, uint32_t lba, uint32_t count);

sd_spi_status_t sd_spi_get_sector_count(uint32_t *out_sector_count);


#endif  // SD_SPI_H
