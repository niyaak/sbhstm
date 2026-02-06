/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body - 16 Button Matrix for SBH Project
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include <stdio.h>
#include <string.h>

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart2;

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);

/**
  * @brief  The application entry point.
  */
int main(void)
{
  /* MCU Configuration */
  HAL_Init();
  SystemClock_Config();

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();

  /* USER CODE BEGIN 2 */
  // Mapping Port dan Pin untuk scanning (Sesuai JSON Node-RED Anda)
  // Row: PA12, PA11, PB5, PB4
  GPIO_TypeDef* rowPorts[] = {GPIOA, GPIOA, GPIOB, GPIOB};
  uint16_t rowPins[] = {GPIO_PIN_12, GPIO_PIN_11, GPIO_PIN_5, GPIO_PIN_4};

  // Col: PB12, PB15, PB13, PB14
  GPIO_TypeDef* colPorts[] = {GPIOB, GPIOB, GPIOB, GPIOB};
  uint16_t colPins[] = {GPIO_PIN_12, GPIO_PIN_15, GPIO_PIN_13, GPIO_PIN_14};

  char msg[64];
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    for (int r = 0; r < 4; r++)
    {
        // 1. Aktifkan Baris ke-r
        HAL_GPIO_WritePin(rowPorts[r], rowPins[r], GPIO_PIN_SET);
        HAL_Delay(1); // Delay singkat untuk stabilitas sinyal

        for (int c = 0; c < 4; c++)
        {
            // 2. Cek apakah Kolom ke-c mendeteksi input HIGH
            if (HAL_GPIO_ReadPin(colPorts[c], colPins[c]) == GPIO_PIN_SET)
            {
                // Hitung ID tombol (1-16)
                int btnNum = (r * 4) + (c + 1);
                
                // 3. Format data menjadi JSON untuk dikirim ke Node-RED
                int len = sprintf(msg, "{\"button\": %d, \"status\": \"pressed\"}\r\n", btnNum);
                HAL_UART_Transmit(&huart2, (uint8_t*)msg, len, HAL_MAX_DELAY);

                // 4. Debounce: Tunggu sampai tombol dilepas
                while(HAL_GPIO_ReadPin(colPorts[c], colPins[c]) == GPIO_PIN_SET) {
                    HAL_Delay(10);
                }
            }
        }
        // 5. Matikan kembali baris sebelum pindah ke baris berikutnya
        HAL_GPIO_WritePin(rowPorts[r], rowPins[r], GPIO_PIN_RESET);
    }
    HAL_Delay(20); // Interval scanning
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief GPIO Initialization Function
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /* Konfigurasi Pin Baris (Output): PA12, PA11, PB5, PB4 */
  GPIO_InitStruct.Pin = GPIO_PIN_12 | GPIO_PIN_11;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = GPIO_PIN_5 | GPIO_PIN_4;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* Konfigurasi Pin Kolom (Input Pull-down): PB12, PB15, PB13, PB14 */
  /* Sesuai konfigurasi Node-RED: "pull": "pulldown" */
  GPIO_InitStruct.Pin = GPIO_PIN_12 | GPIO_PIN_15 | GPIO_PIN_13 | GPIO_PIN_14;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

/**
  * @brief USART2 Initialization Function (Baudrate 115200)
  */
static void MX_USART2_UART_Init(void)
{
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
}

void SystemClock_Config(void)
{
  // Konfigurasi standar clock internal HSI (sesuai kode asli Anda)
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  HAL_RCC_OscConfig(&RCC_OscInitStruct);

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2);
}

void Error_Handler(void)
{
  __disable_irq();
  while (1) {}
}