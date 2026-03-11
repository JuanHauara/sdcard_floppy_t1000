#include "sd_spi.h"

#include "stm32f4xx_hal.h"
#include "board_gpio.h"
#include "utils.h"


extern SPI_HandleTypeDef hspi1;


// MicroSD SPI configuration
#define SD_SPI_PORT						(&hspi1)

/* Timeouts (ms) */
#define SD_SPI_CMD_TIMEOUT_MS			(500u)
#define SD_SPI_INIT_TIMEOUT_MS			(2000u)
#define SD_SPI_READ_TIMEOUT_MS			(500u)
#define SD_SPI_WRITE_TIMEOUT_MS			(800u)

#define SD_TOKEN_START_BLOCK			(0xFEu)
#define SD_TOKEN_DATA_ACCEPTED_MASK		(0x1Fu)
#define SD_TOKEN_DATA_ACCEPTED			(0x05u)

#define CMD0		(0u)
#define CMD8		(8u)
#define CMD9		(9u)
#define CMD16		(16u)
#define CMD17		(17u)
#define CMD24		(24u)
#define CMD55		(55u)
#define CMD58		(58u)
#define ACMD41		(0x80u + 41u)


typedef enum
{
	CARD_NONE = 0,
	CARD_SDSC,
	CARD_SDHC
} card_type_t;


static volatile bool s_ready = false;
static volatile bool s_spi_fault = false;
static card_type_t s_card_type = CARD_NONE;
static uint32_t s_sector_count = 0u;

static uint8_t s_ff_block[SD_SPI_SECTOR_SIZE]; /* tx=0xFF buffer for reads */

static void spi_fault_clear(void)
{
	s_spi_fault = false;
}

static bool spi_fault_is_set(void)
{
	return s_spi_fault;
}


static void cs_high(void)
{
	board_gpio_write_do(SDCARD_SPI_CS, true);
}

static void cs_low(void)
{
	board_gpio_write_do(SDCARD_SPI_CS, false);
}

static uint8_t spi_xfer(uint8_t tx)
{
	uint8_t rx = 0xFFu;
	if (HAL_SPI_TransmitReceive(SD_SPI_PORT, &tx, &rx, 1u, 100u) != HAL_OK)
	{
		s_spi_fault = true;

		return 0u;
	}

	return rx;
}

static void spi_send_ff(uint32_t nbytes)
{
	for (uint32_t i = 0; i < nbytes; i++)
	{
		spi_xfer(0xFFu);
		if (spi_fault_is_set())
		{
			break;
		}
	}
}

static void deselect_card(void)
{
	cs_high();
	spi_xfer(0xFFu); /* extra clocks */
}

static void select_card(void)
{
	cs_low();
}

static bool wait_ready(uint32_t timeout_ms)
{
	uint32_t t0 = HAL_GetTick();
	do
	{
		uint8_t rx = spi_xfer(0xFFu);
		if (spi_fault_is_set())
		{
			return false;
		}

		if (rx == 0xFFu)
		{
			return true;
		}
	} while ((HAL_GetTick() - t0) < timeout_ms);

	return false;
}

static uint8_t send_cmd(uint8_t cmd, uint32_t arg)
{
	uint8_t r1;
	uint8_t crc = 0x01u;
	spi_fault_clear();

	/* ACMD<n> */
	if (cmd & 0x80u)
	{
		cmd &= 0x7Fu;
		r1 = send_cmd(CMD55, 0u);
		if (r1 > 1u)
		{
			return r1;
		}
	}

	deselect_card();
	select_card();

	if (!wait_ready(SD_SPI_CMD_TIMEOUT_MS))
	{
		deselect_card();

		return 0xFFu;
	}

	if (cmd == CMD0)
	{
		crc = 0x95u;
	}
	
	if (cmd == CMD8)
	{
		crc = 0x87u;
	}

	spi_xfer((uint8_t)(0x40u | cmd));
	spi_xfer((uint8_t)(arg >> 24));
	spi_xfer((uint8_t)(arg >> 16));
	spi_xfer((uint8_t)(arg >> 8));
	spi_xfer((uint8_t)(arg));
	spi_xfer(crc);
	if (spi_fault_is_set())
	{
		deselect_card();

		return 0xFFu;
	}

	/* Wait response (bit7 cleared) */
	for (uint32_t i = 0; i < 10u; i++)
	{
		r1 = spi_xfer(0xFFu);
		if (spi_fault_is_set())
		{
			deselect_card();

			return 0xFFu;
		}

		if ((r1 & 0x80u) == 0u)
		{
			return r1;
		}
	}

	return 0xFFu;
}

static bool rcvr_datablock(uint8_t *buf, uint32_t len, uint32_t timeout_ms)
{
	if ((buf == NULL) || (len == 0u) || (len > SD_SPI_SECTOR_SIZE))
	{
		return false;
	}

	uint32_t t0 = HAL_GetTick();
	uint8_t token;

	do
	{
		token = spi_xfer(0xFFu);
		if (spi_fault_is_set())
		{
			return false;
		}

		if (token == SD_TOKEN_START_BLOCK)
		{
			/* Read data while sending 0xFF */
			if (HAL_SPI_TransmitReceive(SD_SPI_PORT, s_ff_block, buf, (uint16_t)len, 200u) != HAL_OK)
			{
				s_spi_fault = true;

				return false;
			}

			/* Discard CRC */
			spi_xfer(0xFFu);
			spi_xfer(0xFFu);
			if (spi_fault_is_set())
			{
				return false;
			}

			return true;
		}
	} while ((HAL_GetTick() - t0) < timeout_ms);

	return false;
}

static bool xmit_datablock(const uint8_t *buf, uint8_t token)
{
	uint8_t resp;

	if (buf == NULL)
	{
		return false;
	}

	if (!wait_ready(SD_SPI_WRITE_TIMEOUT_MS))
	{
		return false;
	}

	spi_xfer(token);
	if (spi_fault_is_set())
	{
		return false;
	}

	if (token != SD_TOKEN_START_BLOCK)
	{
		return false;
	}

	if (HAL_SPI_Transmit(SD_SPI_PORT, (uint8_t*)buf, SD_SPI_SECTOR_SIZE, 500u) != HAL_OK)
	{
		s_spi_fault = true;

		return false;
	}

	/* Dummy CRC */
	spi_xfer(0xFFu);
	spi_xfer(0xFFu);
	if (spi_fault_is_set())
	{
		return false;
	}

	resp = spi_xfer(0xFFu);
	if (spi_fault_is_set())
	{
		return false;
	}

	if ((resp & SD_TOKEN_DATA_ACCEPTED_MASK) != SD_TOKEN_DATA_ACCEPTED)
	{
		return false;
	}

	return true;
}

static sd_spi_status_t read_csd(uint8_t csd[16])
{
	uint8_t r1 = send_cmd(CMD9, 0u);
	if (r1 != 0u)
	{
		deselect_card();

		return SD_SPI_ERR;
	}

	if (!rcvr_datablock(csd, 16u, SD_SPI_READ_TIMEOUT_MS))
	{
		deselect_card();

		return SD_SPI_TIMEOUT;
	}

	deselect_card();

	return SD_SPI_OK;
}

static sd_spi_status_t compute_sector_count(uint32_t *out_count)
{
	if (out_count == NULL)
	{
		return SD_SPI_ERR;
	}

	uint8_t csd[16];
	sd_spi_status_t st = read_csd(csd);
	if (st != SD_SPI_OK)
	{
		return st;
	}

	uint8_t csd_structure = (uint8_t)((csd[0] >> 6) & 0x03u);

	if (csd_structure == 1u)
	{
		/* CSD v2.0 (SDHC/SDXC) */
		uint32_t c_size = ((uint32_t)(csd[7] & 0x3Fu) << 16) | ((uint32_t)csd[8] << 8) | (uint32_t)csd[9];
		*out_count = (c_size + 1u) * 1024u;

		return SD_SPI_OK;
	}
	else if (csd_structure == 0u)
	{
		/* CSD v1.0 (SDSC) */
		uint32_t c_size = ((uint32_t)(csd[6] & 0x03u) << 10) | ((uint32_t)csd[7] << 2) | ((uint32_t)(csd[8] & 0xC0u) >> 6);
		uint32_t c_size_mult = ((uint32_t)(csd[9] & 0x03u) << 1) | ((uint32_t)(csd[10] & 0x80u) >> 7);
		uint32_t read_bl_len = (uint32_t)(csd[5] & 0x0Fu);

		uint32_t block_len = 1u << read_bl_len;
		uint32_t mult = 1u << (c_size_mult + 2u);
		uint32_t capacity_bytes = (c_size + 1u) * mult * block_len;

		*out_count = capacity_bytes / SD_SPI_SECTOR_SIZE;

		return SD_SPI_OK;
	}

	return SD_SPI_ERR;
}

static bool lba_range_is_valid(uint32_t lba, uint32_t count)
{
	if (count == 0u)
	{
		return false;
	}

	uint32_t last_lba = lba + count - 1u;
	if (last_lba < lba)
	{
		return false;
	}

	if (s_sector_count != 0u)
	{
		if ((lba >= s_sector_count) || (last_lba >= s_sector_count))
		{
			return false;
		}
	}

	return true;
}

sd_spi_status_t sd_spi_init(void)
{
	s_ready = false;
	s_card_type = CARD_NONE;
	s_sector_count = 0u;
	spi_fault_clear();

	for (uint32_t i = 0; i < SD_SPI_SECTOR_SIZE; i++)
	{
		s_ff_block[i] = 0xFFu;
	}

	/* Power ON SD card */
	board_gpio_write_do(SDCARD_ON, 1);
	delay_ms(50);

	/* 1) CS high + >=74 clocks (use 80 clocks) */
	cs_high();
	spi_send_ff(10u);
	if (spi_fault_is_set())
	{
		return SD_SPI_ERR;
	}

	/* 2) CMD0 -> IDLE */
	uint32_t t0 = HAL_GetTick();
	uint8_t r1;
	do
	{
		r1 = send_cmd(CMD0, 0u);
		deselect_card();
		if (r1 == 0x01u)
		{
			break;
		}
	} while ((HAL_GetTick() - t0) < SD_SPI_INIT_TIMEOUT_MS);

	if (r1 != 0x01u)
	{
		return SD_SPI_TIMEOUT;
	}

	/* 3) CMD8 -> detect SDv2 */
	r1 = send_cmd(CMD8, 0x000001AAu);
	uint8_t r7[4] = {0};

	if (r1 == 0x01u)
	{
		/* Read R7 */
		r7[0] = spi_xfer(0xFFu);
		r7[1] = spi_xfer(0xFFu);
		r7[2] = spi_xfer(0xFFu);
		r7[3] = spi_xfer(0xFFu);
		deselect_card();
		if (spi_fault_is_set())
		{
			return SD_SPI_ERR;
		}

		if (r7[2] != 0x01u || r7[3] != 0xAAu)
		{
			return SD_SPI_ERR;
		}

		/* ACMD41 with HCS */
		t0 = HAL_GetTick();
		do
		{
			r1 = send_cmd(ACMD41, 0x40000000u);
			deselect_card();
			if (r1 == 0u)
			{
				break;
			}
		} while ((HAL_GetTick() - t0) < SD_SPI_INIT_TIMEOUT_MS);

		if (r1 != 0u)
		{
			return SD_SPI_TIMEOUT;
		}

		/* CMD58 read OCR */
		r1 = send_cmd(CMD58, 0u);
		if (r1 != 0u)
		{
			deselect_card();

			return SD_SPI_ERR;
		}

		uint8_t ocr[4];
		ocr[0] = spi_xfer(0xFFu);
		ocr[1] = spi_xfer(0xFFu);
		ocr[2] = spi_xfer(0xFFu);
		ocr[3] = spi_xfer(0xFFu);
		deselect_card();
		if (spi_fault_is_set())
		{
			return SD_SPI_ERR;
		}

		/* CCS bit (OCR[0] bit6) */
		if (ocr[0] & 0x40u)
		{
			s_card_type = CARD_SDHC;
		}
		else
		{
			s_card_type = CARD_SDSC;
		}
	}
	else
	{
		/* SDv1 (or MMC). We try SDv1 path: ACMD41 without HCS */
		deselect_card();

		t0 = HAL_GetTick();
		do
		{
			r1 = send_cmd(ACMD41, 0u);
			deselect_card();
			if (r1 == 0u)
			{
				break;
			}
		} while ((HAL_GetTick() - t0) < SD_SPI_INIT_TIMEOUT_MS);

		if (r1 != 0u)
		{
			return SD_SPI_TIMEOUT;
		}

		s_card_type = CARD_SDSC;
	}

	/* 4) For SDSC set block length = 512 */
	if (s_card_type == CARD_SDSC)
	{
		r1 = send_cmd(CMD16, SD_SPI_SECTOR_SIZE);
		deselect_card();
		if (r1 != 0u)
		{
			return SD_SPI_ERR;
		}
	}

	/* Cache sector count (optional, but useful) */
	compute_sector_count(&s_sector_count);

	s_ready = true;

	return SD_SPI_OK;
}

bool sd_spi_is_ready(void)
{
	return s_ready;
}

sd_spi_status_t sd_spi_read_blocks(uint8_t *dst, uint32_t lba, uint32_t count)
{
	if (!s_ready || (s_card_type == CARD_NONE) || (dst == NULL) || (count == 0u))
	{
		return SD_SPI_NOT_READY;
	}

	if (!lba_range_is_valid(lba, count))
	{
		return SD_SPI_ERR;
	}

	spi_fault_clear();

	for (uint32_t i = 0; i < count; i++)
	{
		uint32_t block_index = lba + i;
		uint32_t addr = block_index;
		if (s_card_type != CARD_SDHC)
		{
			if (block_index > (UINT32_MAX / SD_SPI_SECTOR_SIZE))
			{
				return SD_SPI_ERR;
			}

			addr = block_index * SD_SPI_SECTOR_SIZE;
		}

		uint8_t r1 = send_cmd(CMD17, addr);
		if (r1 != 0u)
		{
			deselect_card();

			return SD_SPI_ERR;
		}

		if (!rcvr_datablock(dst + (i * SD_SPI_SECTOR_SIZE), SD_SPI_SECTOR_SIZE, SD_SPI_READ_TIMEOUT_MS))
		{
			deselect_card();
			if (spi_fault_is_set())
			{
				return SD_SPI_ERR;
			}

			return SD_SPI_TIMEOUT;
		}

		deselect_card();
	}

	return SD_SPI_OK;
}

sd_spi_status_t sd_spi_write_blocks(const uint8_t *src, uint32_t lba, uint32_t count)
{
	if (!s_ready || (s_card_type == CARD_NONE) || (src == NULL) || (count == 0u))
	{
		return SD_SPI_NOT_READY;
	}

	if (!lba_range_is_valid(lba, count))
	{
		return SD_SPI_ERR;
	}

	spi_fault_clear();

	for (uint32_t i = 0; i < count; i++)
	{
		uint32_t block_index = lba + i;
		uint32_t addr = block_index;
		if (s_card_type != CARD_SDHC)
		{
			if (block_index > (UINT32_MAX / SD_SPI_SECTOR_SIZE))
			{
				return SD_SPI_ERR;
			}

			addr = block_index * SD_SPI_SECTOR_SIZE;
		}

		uint8_t r1 = send_cmd(CMD24, addr);
		if (r1 != 0u)
		{
			deselect_card();

			return SD_SPI_ERR;
		}

		if (!xmit_datablock(src + (i * SD_SPI_SECTOR_SIZE), SD_TOKEN_START_BLOCK))
		{
			deselect_card();
			if (spi_fault_is_set())
			{
				return SD_SPI_ERR;
			}

			return SD_SPI_ERR;
		}

		/* Wait not busy */
		if (!wait_ready(SD_SPI_WRITE_TIMEOUT_MS))
		{
			deselect_card();
			if (spi_fault_is_set())
			{
				return SD_SPI_ERR;
			}

			return SD_SPI_TIMEOUT;
		}

		deselect_card();
	}

	return SD_SPI_OK;
}

sd_spi_status_t sd_spi_get_sector_count(uint32_t *out_sector_count)
{
	if (out_sector_count == NULL)
	{
		return SD_SPI_ERR;
	}

	if (!s_ready || (s_card_type == CARD_NONE))
	{
		return SD_SPI_NOT_READY;
	}

	spi_fault_clear();

	if (s_sector_count == 0u)
	{
		sd_spi_status_t st = compute_sector_count(&s_sector_count);
		if (st != SD_SPI_OK)
		{
			return st;
		}

		if (spi_fault_is_set())
		{
			return SD_SPI_ERR;
		}
	}

	*out_sector_count = s_sector_count;

	return SD_SPI_OK;
}

