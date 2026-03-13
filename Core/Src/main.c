/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "fatfs.h"
#include "usb_device.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h>

#include "debug_log.h"
#include "core/floppy_drive.h"
#include "floppy_emu.h"
#include "iface/toshiba_fdd_iface.h"
#include "utils.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define APP_LOG_TRACK0_MFM    0

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
SPI_HandleTypeDef hspi1;

TIM_HandleTypeDef htim2;

/* USER CODE BEGIN PV */
static floppy_drive_t s_floppy_drive;
static floppy_emu_t s_floppy_emu;
static toshiba_fdd_iface_t s_toshiba_fdd_iface;
static toshiba_fdd_signal_polarity_t s_toshiba_polarity;
static bool s_prev_step_active;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI1_Init(void);
static void MX_TIM2_Init(void);
/* USER CODE BEGIN PFP */
static void app_init(void);
static void app_log_drive_state(void);
static void app_log_fixed_image(void);
static void app_log_boot_sector(void);
#if APP_LOG_TRACK0_MFM
static void app_log_track0_mfm(void);
#endif
static void app_write_drive_outputs_provisional(void);
static void app_service_logical_iface(void);
static void app_service(void);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
static void app_log_drive_state(void)
{
  floppy_drive_status_t drive_status;

  floppy_drive_get_status(&s_floppy_drive, &drive_status);

  DEBUG_SERIAL_LOG("drive: cyl=%u head=%u media=%u wprot=%u changed=%u selected=%u motor=%u track0=%u\r\n",
                   drive_status.cylinder,
                   drive_status.head,
                   (unsigned int)drive_status.media_present,
                   (unsigned int)drive_status.write_protected,
                   (unsigned int)drive_status.disk_changed,
                   (unsigned int)drive_status.selected,
                   (unsigned int)drive_status.motor_on,
                   (unsigned int)drive_status.track0);
}

static void app_write_drive_outputs_provisional(void)
{
  floppy_drive_status_t drive_status;
  toshiba_fdd_logical_outputs_t logical_outputs;

  /* Provisional bring-up subset: only TRACK0 and WPROTC are driven here. */
  floppy_drive_get_status(&s_floppy_drive, &drive_status);

  memset(&logical_outputs, 0, sizeof(logical_outputs));
  logical_outputs.track0_active = drive_status.track0;
  logical_outputs.track0_valid = true;
  logical_outputs.wprotc_active = drive_status.write_protected;
  logical_outputs.wprotc_valid = true;

  (void)toshiba_fdd_iface_write_outputs(&logical_outputs, &s_toshiba_polarity);
}

static void app_service_logical_iface(void)
{
  toshiba_fdd_logical_inputs_t logical_inputs;
  bool step_moved;

  if (!toshiba_fdd_iface_decode_inputs(&s_toshiba_fdd_iface,
                                       &s_toshiba_polarity,
                                       &logical_inputs))
  {
    s_prev_step_active = false;
    return;
  }

  if (logical_inputs.drive_select_valid)
  {
    floppy_drive_set_selected(&s_floppy_drive, logical_inputs.drive_select);
  }

  if (logical_inputs.motor_on_valid)
  {
    floppy_drive_set_motor_on(&s_floppy_drive, logical_inputs.motor_on);
  }

  if (logical_inputs.side_valid)
  {
    (void)floppy_drive_set_head(&s_floppy_drive, logical_inputs.side_1_selected ? 1u : 0u);
  }

  step_moved = false;
  if (!logical_inputs.step_active_valid)
  {
    s_prev_step_active = false;
    return;
  }

  if (logical_inputs.step_active && !s_prev_step_active)
  {
    if (!logical_inputs.direction_valid)
    {
      DEBUG_SERIAL_LOG("drive step edge ignored: fdcdrc polarity pending\r\n");
      app_log_drive_state();
      app_write_drive_outputs_provisional();
      s_prev_step_active = logical_inputs.step_active;
      return;
    }

    if (logical_inputs.direction_towards_center)
    {
      step_moved = floppy_drive_step_towards_center(&s_floppy_drive);
    }
    else
    {
      step_moved = floppy_drive_step_towards_track0(&s_floppy_drive);
    }

    DEBUG_SERIAL_LOG("drive step dir=%s moved=%u\r\n",
                     logical_inputs.direction_towards_center ? "center" : "track0",
                     (unsigned int)step_moved);
    app_log_drive_state();
    app_write_drive_outputs_provisional();
  }

  s_prev_step_active = logical_inputs.step_active;
}

#if APP_LOG_TRACK0_MFM
static void app_log_track0_mfm(void)
{
  static uint16_t s_track_words[FLOPPY_EMU_MAX_TRACK_WORDS];
  floppy_emu_track_info_t track_info;
  floppy_emu_status_t status;
  uint32_t words_to_log;
  uint32_t index;

  status = floppy_drive_build_current_track(&s_floppy_drive,
                                            &s_floppy_emu,
                                            s_track_words,
                                            FLOPPY_EMU_MAX_TRACK_WORDS,
                                            &track_info);
  if (status != FLOPPY_EMU_STATUS_OK)
  {
    DEBUG_SERIAL_LOG("track 0.0 build failed: %s\r\n", floppy_emu_status_str(status));
    return;
  }

  DEBUG_SERIAL_LOG("track 0.0: rate=%u rpm=%u words=%lu gap4a=%lu gap3=%lu pre_index=%lu\r\n",
                   track_info.data_rate_kbps,
                   track_info.rpm,
                   (unsigned long)track_info.track_word_count,
                   (unsigned long)track_info.gap_4a_words,
                   (unsigned long)track_info.gap_3_words,
                   (unsigned long)track_info.pre_index_gap_words);

  words_to_log = (track_info.track_word_count < 8u) ? track_info.track_word_count : 8u;
  DEBUG_SERIAL_LOG("track 0.0 first words:");
  for (index = 0u; index < words_to_log; index++)
  {
    DEBUG_SERIAL_LOG(" %04X", s_track_words[index]);
  }
  DEBUG_SERIAL_LOG("\r\n");
}
#endif

static void app_log_boot_sector(void)
{
  uint8_t boot_sector[FLOPPY_EMU_SECTOR_SIZE];
  uint16_t boot_signature;
  floppy_emu_status_t status;

  status = floppy_emu_read_lba_sector(&s_floppy_emu, 0u, boot_sector, sizeof(boot_sector));
  if (status != FLOPPY_EMU_STATUS_OK)
  {
    DEBUG_SERIAL_LOG("boot sector read failed: %s\r\n", floppy_emu_status_str(status));
    return;
  }

  boot_signature = (uint16_t)boot_sector[510]
                 | ((uint16_t)boot_sector[511] << 8);

  DEBUG_SERIAL_LOG("boot jump=%02X %02X %02X oem='%.8s' sig=%04X\r\n",
                   boot_sector[0],
                   boot_sector[1],
                   boot_sector[2],
                   &boot_sector[3],
                   boot_signature);
}

static void app_log_fixed_image(void)
{
  const floppy_emu_geometry_t *geometry = floppy_emu_get_geometry(&s_floppy_emu);

  if (geometry == NULL)
  {
    DEBUG_SERIAL_LOG("geometry unavailable\r\n");
    return;
  }

  DEBUG_SERIAL_LOG("fixed image: %s\r\n", floppy_emu_get_path(&s_floppy_emu));
  DEBUG_SERIAL_LOG("geometry: %lu bytes, %u cyl, %u heads, %u spt, %u bps\r\n",
                   (unsigned long)s_floppy_emu.size_bytes,
                   geometry->cylinders,
                   geometry->heads,
                   geometry->sectors_per_track,
                   geometry->bytes_per_sector);

  app_log_boot_sector();
  app_log_drive_state();

#if APP_LOG_TRACK0_MFM
  app_log_track0_mfm();
#endif
}

static void app_init(void)
{
  const toshiba_fdd_inputs_t *inputs;
  floppy_emu_status_t status;

  if (HAL_TIM_Base_Start(&htim2) != HAL_OK)
  {
    DEBUG_SERIAL_LOG("tim2 start failed\r\n");
    return;
  }

  delay_ms(200u);
  DEBUG_SERIAL_LOG("\r\nsdcard_floppy_t1000 boot\r\n");

  toshiba_fdd_iface_init(&s_toshiba_fdd_iface);
  inputs = toshiba_fdd_iface_get_inputs(&s_toshiba_fdd_iface);
  if (inputs != NULL)
  {
    DEBUG_SERIAL_LOG("pj5 init raw fd_sela=%u mona=%u lowdns=%u fdcdrc=%u step=%u wgate=%u side=%u wdata=%u\r\n",
                     (unsigned int)inputs->fd_sela_raw,
                     (unsigned int)inputs->mona_raw,
                     (unsigned int)inputs->lowdns_raw,
                     (unsigned int)inputs->fdcdrc_raw,
                     (unsigned int)inputs->step_raw,
                     (unsigned int)inputs->wgate_raw,
                     (unsigned int)inputs->side_raw,
                     (unsigned int)inputs->wdata_raw);
  }
  toshiba_fdd_iface_load_initial_guess_polarity(&s_toshiba_polarity);
  DEBUG_SERIAL_LOG("pj5 polarity loaded: initial_guess\r\n");
  DEBUG_SERIAL_LOG("pj5 polarity pending: media fdcdrc rdda\r\n");

  status = floppy_emu_mount_fixed_image(&s_floppy_emu);
  if (status != FLOPPY_EMU_STATUS_OK)
  {
    DEBUG_SERIAL_LOG("fixed image mount failed: %s\r\n", floppy_emu_status_str(status));
    return;
  }

  floppy_drive_init(&s_floppy_drive, &s_floppy_emu);
  app_write_drive_outputs_provisional();
  app_log_fixed_image();
}

static void app_service(void)
{
  uint32_t changed_mask;

  changed_mask = toshiba_fdd_iface_poll_inputs(&s_toshiba_fdd_iface);
  if (changed_mask != TOSHIBA_FDD_CHANGED_NONE)
  {
    toshiba_fdd_iface_log_inputs(&s_toshiba_fdd_iface, changed_mask);
  }

  app_service_logical_iface();
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_SPI1_Init();
  MX_USB_DEVICE_Init();
  MX_TIM2_Init();
  MX_FATFS_Init();
  /* USER CODE BEGIN 2 */
  app_init();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    app_service();
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 83;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 4294967295;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3|GPIO_PIN_4, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, GPIO_PIN_RESET);

  /*Configure GPIO pins : PA3 PA4 */
  GPIO_InitStruct.Pin = GPIO_PIN_3|GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PC4 */
  GPIO_InitStruct.Pin = GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
