/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "can.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "BasicUart.h"
#include "SystemInfo.h"
#include "outputs.h"
#include "error.h"
#include "inputs.h"
#include "my_math.h"
#include "batteriemanagement.h"
#include "BatteriemanagementSystem.h"
#include "imd.h"
#include "millis.h"
#include <math.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
CAN_RxHeaderTypeDef RxMessage;
uint8_t RxData[8], can_change = 0;
static volatile uint16_t rising = 0;
static volatile uint16_t falling = 0;
volatile uint8_t millisekunden_flag_1 = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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
  uint8_t data[36] = {0};

	// Definiere Variablen fuer Main-Funktion
	uint8_t TxData[8], OutData[4], InData[3], status;
  	CAN_FilterTypeDef sFilterConfig;

  	// Erstelle Can-Nachrichten
  	CAN_TxHeaderTypeDef TxMessage = {0x123, 0, CAN_RTR_DATA, CAN_ID_STD, 8, DISABLE};
  	CAN_TxHeaderTypeDef TxOutput = {BMS_CAN_DIGITAL_OUT, 0, CAN_RTR_DATA, CAN_ID_STD, 4, DISABLE};
  	CAN_TxHeaderTypeDef TxInput = {BMS_CAN_DIGITAL_IN, 0, CAN_RTR_DATA, CAN_ID_STD, 3, DISABLE};
  	CAN_TxHeaderTypeDef TxVoltage11 = {BMS_CAN_ZELLEN11, 0, CAN_RTR_DATA, CAN_ID_STD, 8, DISABLE};
  	CAN_TxHeaderTypeDef TxVoltage12 = {BMS_CAN_ZELLEN12, 0, CAN_RTR_DATA, CAN_ID_STD, 8, DISABLE};
  	CAN_TxHeaderTypeDef TxVoltage13 = {BMS_CAN_ZELLEN13, 0, CAN_RTR_DATA, CAN_ID_STD, 8, DISABLE};
  	CAN_TxHeaderTypeDef TxTemperature11 = {BMS_CAN_TEMPERATUR11, 0, CAN_RTR_DATA, CAN_ID_STD, 8, DISABLE};
  	CAN_TxHeaderTypeDef TxTemperature12 = {BMS_CAN_TEMPERATUR12, 0, CAN_RTR_DATA, CAN_ID_STD, 8, DISABLE};
  	CAN_TxHeaderTypeDef TxTemperature13 = {BMS_CAN_TEMPERATUR13, 0, CAN_RTR_DATA, CAN_ID_STD, 8, DISABLE};
  	CAN_TxHeaderTypeDef TxTemperature14 = {BMS_CAN_TEMPERATUR14, 0, CAN_RTR_DATA, CAN_ID_STD, 8, DISABLE};
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_CAN1_Init();
  MX_SPI4_Init();
  MX_USART2_UART_Init();
  MX_ADC1_Init();
  MX_TIM1_Init();
  MX_TIM4_Init();
  MX_SPI1_Init();
  MX_CAN3_Init();
  /* USER CODE BEGIN 2 */

	/* Schreibe Resetquelle auf die Konsole */
#ifdef DEBUG
	printResetSource(readResetSource());

	/* Teste serielle Schnittstelle*/
	#define TEST_STRING_UART		"\nUART2 Transmitting in polling mode, Hello Diveturtle93!\n"
	uartTransmit(TEST_STRING_UART, sizeof(TEST_STRING_UART));

  	// Sammel Systeminformationen
  	collectSystemInfo();
#endif

	// Leds Testen
	testPCB_Leds();

  	// Lese alle Eingaenge
  	readall_inputs();

  	// BMS initialisieren
  	bms_init();

  	// Start timer
	if (HAL_TIM_Base_Start_IT(&htim1) != HAL_OK);
	if (HAL_TIM_IC_Start_IT(&htim1, TIM_CHANNEL_1) != HAL_OK);
	if (HAL_TIM_IC_Start_IT(&htim1, TIM_CHANNEL_2) != HAL_OK);

  	// Starte CAN Bus
  	if((status = HAL_CAN_Start(&hcan3)) != HAL_OK)
  	{
  		/* Start Error */
  		hal_error(status);
  		Error_Handler();
  	}
  	uartTransmit("CAN START\n", 10);

  	// Aktiviere Interrupts für CAN Bus
  	if((status = HAL_CAN_ActivateNotification(&hcan3, CAN_IT_RX_FIFO0_MSG_PENDING)) != HAL_OK)
  	{
  		/* Notification Error */
  		hal_error(status);
  		Error_Handler();
  	}
  	uartTransmit("Send Message\n", 13);

  	// Filter Bank initialisieren um Daten zu empfangen
    sFilterConfig.FilterBank = 0;
    sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
    sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
    sFilterConfig.FilterIdHigh = 0x0000;
    sFilterConfig.FilterIdLow = 0x0000;
    sFilterConfig.FilterMaskIdHigh = 0x0000;
    sFilterConfig.FilterMaskIdLow = 0x0000;
    sFilterConfig.FilterFIFOAssignment = 0;
    sFilterConfig.FilterActivation = ENABLE;

    // Filter Bank schreiben
    if((status = HAL_CAN_ConfigFilter(&hcan3, &sFilterConfig)) != HAL_OK)
    {
    	/* Filter configuration Error */
  		hal_error(status);
  		Error_Handler();
    }

    // Test CAN Nachricht beschreiben
    for (uint8_t j = 0; j < 8; j++)
    {
    	TxData[j] = (j + 1);
    }

	if (!(sdc_in.sdcinput && 0b00001111))										// SDC OK; Motor, BTB, IMD und HVIL OK
	{
		#define SDC_STRING_ERROR			"\nSDC ist nicht geschlossen"
		uartTransmit(SDC_STRING_ERROR, sizeof(SDC_STRING_ERROR));

		// LEDs setzen bei SDC Fehler
		leuchten_out.GreenLed = 0;
		leuchten_out.RedLed = 1;
		leuchten_out.AkkuErrorLed = 0;
		HAL_GPIO_WritePin(GREEN_LED_GPIO_Port, GREEN_LED_Pin, leuchten_out.GreenLed);
		HAL_GPIO_WritePin(RED_LED_GPIO_Port, RED_LED_Pin, leuchten_out.RedLed);
		HAL_GPIO_WritePin(AKKU_LED_GPIO_Port, AKKU_LED_Pin, leuchten_out.AkkuErrorLed);

		// Ausgabe welcher Fehler vorhanden
		// Motorsteuergeraet Fehler
		if(!(sdc_in.MotorSDC == 1))
		{
			#define SDC_STRING_MOTOR		"\nSDC Motor hat einen Fehler und ist offen"
			uartTransmit(SDC_STRING_MOTOR, sizeof(SDC_STRING_MOTOR));
		}

		// BamoCar Fehler
		if (!(sdc_in.BTB_SDC == 1))
		{
			#define SDC_STRING_BTB			"\nSDC BTB hat einen Fehler und ist offen"
			uartTransmit(SDC_STRING_BTB, sizeof(SDC_STRING_BTB));
		}

		// HVIL Fehler
		if (!(sdc_in.HVIL == 1))
		{
			#define SDC_STRING_HVIL			"\nSDC HVIL ist nicht geschlossen"
			uartTransmit(SDC_STRING_HVIL, sizeof(SDC_STRING_HVIL));
		}

		// IMD Fehler
		if (!(sdc_in.IMD_OK_IN == 1))
		{
			#define SDC_STRING_IMD			"\nSDC IMD hat einen Fehler"
			uartTransmit(SDC_STRING_IMD, sizeof(SDC_STRING_IMD));
		}
	}
	else
	{
		// Keine Fehler, LEDs fuer OK setzen
		system_out.AmsOK = 1;
		HAL_GPIO_WritePin(AMS_OK_GPIO_Port, AMS_OK_Pin, system_out.AmsOK);
		leuchten_out.GreenLed = 1;
		HAL_GPIO_WritePin(GREEN_LED_GPIO_Port, GREEN_LED_Pin, leuchten_out.GreenLed);

		// Ausgabe SDC geschlossen
		#define SDC_STRING_OK				"\nSDC ist geschlossen"
		uartTransmit(SDC_STRING_OK, sizeof(SDC_STRING_OK));
	}

	uint32_t lasttime100ms = millis();
	uint32_t lasttime200ms = millis();
	uint32_t lasttime500ms = millis();
	uint32_t lasttime1s = millis();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		// Task wird alle 100 Millisekunden ausgefuehrt
		if (millis() - 100 > lasttime100ms)
		{
			// Lese alle Eingaenge
			readall_inputs();

			lasttime100ms = millis();
		}

		// Task wird alle 200 Millisekunden ausgefuehrt
		if (millis() - 250 > lasttime200ms)
		{
			// Lese IMD ein
			//imd_status();

			lasttime200ms = millis();
		}

		// Task wird alle 250 Millisekunden ausgefuehrt
		if (millis() - 500 > lasttime500ms)
		{
			// Daten fuer Ausgaenge zusammenfuehren
			OutData[0] = system_out.systemoutput;
			OutData[1] = highcurrent_out.high_out;
			OutData[2] = leuchten_out.ledoutput;
			OutData[3] = komfort_out.komfortoutput;
	
			// Sende Nachricht digitale Ausgaenge
			status = HAL_CAN_AddTxMessage(&hcan3, &TxOutput, OutData, (uint32_t *)CAN_TX_MAILBOX0);
			hal_error(status);

			// Daten fuer Eingaenge zusammenfuehren
			InData[0] = system_in.systeminput;
			InData[1] = sdc_in.sdcinput;
			InData[2] = komfort_in.komfortinput;
	
			// Sende Nachricht digitale Eingaenge
			status = HAL_CAN_AddTxMessage(&hcan3, &TxInput, InData, (uint32_t *)CAN_TX_MAILBOX1);
			hal_error(status);

			// Sende Nachricht Zellspannung 1 - 4
			status = HAL_CAN_AddTxMessage(&hcan3, &TxVoltage11, &data[0], (uint32_t *)CAN_TX_MAILBOX0);
			hal_error(status);

			HAL_Delay(1);

			// Sende Nachricht Zellspannung 5 - 8
			status = HAL_CAN_AddTxMessage(&hcan3, &TxVoltage12, &data[6], (uint32_t *)CAN_TX_MAILBOX1);
			hal_error(status);

			// Sende Nachricht Zellspannung 9 - 12
			status = HAL_CAN_AddTxMessage(&hcan3, &TxVoltage13, &data[12], (uint32_t *)CAN_TX_MAILBOX2);
			hal_error(status);

			// Sende Nachricht Zelltemperaturen 1 - 4
			status = HAL_CAN_AddTxMessage(&hcan3, &TxTemperature11, &data[0], (uint32_t *)CAN_TX_MAILBOX0);
			hal_error(status);

			HAL_Delay(1);

			// Sende Nachricht Zelltemperaturen 5 - 8
			status = HAL_CAN_AddTxMessage(&hcan3, &TxTemperature12, &data[6], (uint32_t *)CAN_TX_MAILBOX1);
			hal_error(status);

			// Sende Nachricht Zelltemperaturen 9 - 12
			status = HAL_CAN_AddTxMessage(&hcan3, &TxTemperature13, &data[12], (uint32_t *)CAN_TX_MAILBOX2);
			hal_error(status);

			// Sende Nachricht Zelltemperaturen 13 - 16
			status = HAL_CAN_AddTxMessage(&hcan3, &TxTemperature14, &data[18], (uint32_t *)CAN_TX_MAILBOX2);
			hal_error(status);

			HAL_Delay(1);

			lasttime500ms = millis();
		}

		// Task wird alle 500 Millisekunden ausgefuehrt
		if (millis() - 1000 > lasttime1s)
		{
			// Sende Nachricht Dummy
			status = HAL_CAN_AddTxMessage(&hcan3, &TxMessage, TxData, (uint32_t *)CAN_TX_MAILBOX2);
			hal_error(status);

			HAL_GPIO_TogglePin(BLUE_LED_GPIO_Port, BLUE_LED_Pin);

			lasttime1s = millis();
		}
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
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 432;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 8;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Activate the Over-Drive mode
  */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_7) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
// Interrupts
// Can-Interrupt: Nachricht wartet
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
	HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &RxMessage, RxData);
	can_change = 1;
}

// Timer-Interrupt: Timer ist uebergelaufen
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	// Timer IMD
	if (htim == &htim1)
	{
		if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
		{
			rising = calculateMovingAverage(rising, HAL_TIM_ReadCapturedValue(&htim1, TIM_CHANNEL_1), 64);
		}
		else if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2)
		{
			falling = calculateMovingAverage(falling, HAL_TIM_ReadCapturedValue(&htim1, TIM_CHANNEL_2), 64);
		}
	}
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();														// Interrupts deaktivieren

	// Schalte Fehler LED ein
	leuchten_out.RedLed = 1;												// Setze Variable
	HAL_GPIO_WritePin(RED_LED_GPIO_Port, RED_LED_Pin, leuchten_out.RedLed);	// Fehler LED einschalten

	// Schalte Ok LED aus
	leuchten_out.GreenLed = 0;												// Zuruechsetzen Variable
	HAL_GPIO_WritePin(GREEN_LED_GPIO_Port, GREEN_LED_Pin, leuchten_out.GreenLed);// Fehler LED ausschalten

	// Sende Nachricht auf Uart-Interface
#ifdef DEBUG
#define STRING_ERROR_HANDLER			"Error Handler wird ausgefuehrt!!!"
	uartTransmit(STRING_ERROR_HANDLER, sizeof(STRING_ERROR_HANDLER));
#endif
	// Beginne Endlosschleife nachdem Fehler aufgetreten ist
	while (1)
	{

	}
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
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
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
