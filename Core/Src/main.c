/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @author			: Rafael Hernández Marrero (rafaelh96@gmail.com)
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "keypad.h"
#include "rtc.h"
#include "lcd.h"
#include <stdio.h>
#include <stdint.h>
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
I2C_HandleTypeDef hi2c1;
I2C_HandleTypeDef hi2c2;

TIM_HandleTypeDef htim6;

/* USER CODE BEGIN PV */

Keypad_HandleTypeDef keypad;
LCD_HandleTypeDef lcd;
RTC_HandleTypeDef rtc;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_I2C2_Init(void);
static void MX_TIM6_Init(void);
/* USER CODE BEGIN PFP */

Keypad_HandleTypeDef keypad_init(void);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/**
 * @fn void number_to_string(uint8_t, char*)
 * @brief This function converts an integer value into its corresponding character.
 * @param num Integer value.
 * @param buf Character.
 */

void number_to_string(uint8_t num, char *buf){
	if (num < 10){
		buf[0] = '0';
		buf[1] = num+48;
	}else if (num >= 10 && num <99){
		buf[0] = (num / 10) + 48;
		buf[1] = (num % 10) + 48;
	}
}

/**
 * @fn char* time_to_string(RTC_time_t*)
 * @brief This function converts the rtc_time struct into a string with the format hh:mm:ss.
 * @param rtc_time Time handler.
 * @return char*
 */

char *time_to_string(RTC_time_t *rtc_time){
	static char buf[9];
	buf[2] = ':';
	buf[5] = ':';

	number_to_string(rtc_time->hours, buf);
	number_to_string(rtc_time->minutes, &buf[3]);
	number_to_string(rtc_time->seconds, &buf[6]);

	buf[8] = '\0';

	return buf;
}

/**
 * @fn char* date_to_string(RTC_date_t*)
 * @brief This function converts the rtc_date struct into a string with the format dd/mm/yyyy.
 * @param rtc_date Date handler.
 * @return char*
 */
char *date_to_string(RTC_date_t *rtc_date){
	static char buf[11];

	number_to_string(rtc_date->date, buf);
	buf[2] = '/';
	number_to_string(rtc_date->month, &buf[3]);
	buf[5] = '/';
	buf[6] = '2';
	buf[7] = '0';
	number_to_string(rtc_date->year, &buf[8]);
	buf[10] = '\0';

	return buf;
}

/**
 * @fn uint8_t num_returned()
 * @brief This function retrieves the keypad number pressed and prints it on the lcd.
 * This function can distinguishes between one decimal number and two decimal numbers.
 * The only way to accept a number is by pressing the "D" key.
 * @return uint8_t
 */

uint8_t num_returned(){

	uint8_t key;
	uint8_t num_buf[2];
	char data[2];
	uint8_t num;

	lcd_send_command(&lcd, LCD_CMD_Don_nC_BL);
	while(!key_selected(&keypad, &key));
	num_buf[0] = key;
	sprintf(data, "%d", key);
	lcd_print_string(&lcd, data);

	while(!key_selected(&keypad, &key));
	if(key != 'D'){
		num_buf[1] = key;
		sprintf(data, "%d", key);
		lcd_print_string(&lcd, data);
		while(!key_selected(&keypad, &key));
		if(key == 'D'){
			num = (num_buf[0] * 10) + num_buf[1];
		}else{
			return 120;
		}
	}else{
		num = num_buf[0];
	}

	return num;

}

/**
 * @fn void select_day(RTC_date_t*)
 * @brief This function let the user choose the day among the list of the week.
 * "D" is to select the day and "C" is to continuous to the next day.
 * @param current_date Date handler.
 */

void select_day(RTC_date_t *current_date){

	lcd_display_clear(&lcd);
	lcd_display_return_home(&lcd);

	char *day_of_week[] = { "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday" };
	uint8_t key = 0;
	int32_t counter = 0;
	uint8_t exit = 0;

	while (!exit){
		lcd_print_string(&lcd, "Day: ");
		lcd_print_string(&lcd, day_of_week[counter]);

		while(!key_selected(&keypad, &key));
		if(key == 'D'){
			current_date->day = (counter+1);
			exit = 1;
		}else if(key == 'C'){
			(counter == 6) ? (counter = 0) : (counter++);
		}

		lcd_display_clear(&lcd);
		lcd_display_return_home(&lcd);

	}


}


/**
 * @fn void config_menu()
 * @brief This function sets the time and date parameters.
 * The code will keep executing until the returned value is in its own boundaries.
 * @retval none
 */
void config_menu(){
	RTC_time_t current_time;
	RTC_date_t current_date;

	// Time format will be fixed at 24-hours format
	current_time.time_format = TIME_FORMAT_24HRS;

	select_day(&current_date);
	uint8_t temp = 0;

	lcd_display_clear(&lcd);
	lcd_display_return_home(&lcd);

	lcd_print_string(&lcd, "Date: ");
	temp = num_returned();

	while ((temp <= 0) || (temp >=32)){
		lcd_display_clear(&lcd);
		lcd_display_return_home(&lcd);
		lcd_send_command(&lcd, LCD_CMD_Don_nC_nBL);
		lcd_print_string(&lcd, "Not valid!");
		HAL_Delay(2000);
		lcd_display_clear(&lcd);
		lcd_display_return_home(&lcd);
		lcd_print_string(&lcd, "Date: ");
		temp = num_returned();
	}

	current_date.date = temp;
	temp = 0;

	lcd_display_clear(&lcd);
	lcd_display_return_home(&lcd);

	lcd_print_string(&lcd, "Month: ");
	temp = num_returned();

	while ((temp <= 0) || (temp >=13)){
		lcd_display_clear(&lcd);
		lcd_display_return_home(&lcd);
		lcd_send_command(&lcd, LCD_CMD_Don_nC_nBL);
		lcd_print_string(&lcd, "Not valid!");
		HAL_Delay(2000);
		lcd_display_clear(&lcd);
		lcd_display_return_home(&lcd);
		lcd_print_string(&lcd, "Month: ");
		temp = num_returned();
	}

	current_date.month = temp;
	temp = 0;

	lcd_display_clear(&lcd);
	lcd_display_return_home(&lcd);

	lcd_print_string(&lcd, "Year: 20");
	temp = num_returned();

	while ((temp <= 0) || (temp >=99)){
		lcd_display_clear(&lcd);
		lcd_display_return_home(&lcd);
		lcd_send_command(&lcd, LCD_CMD_Don_nC_nBL);
		lcd_print_string(&lcd, "Not valid!");
		HAL_Delay(2000);
		lcd_display_clear(&lcd);
		lcd_display_return_home(&lcd);
		lcd_print_string(&lcd, "Year: 20");
		temp = num_returned();
	}

	current_date.year = temp;
	temp = 0;

	lcd_display_clear(&lcd);
	lcd_display_return_home(&lcd);

	lcd_print_string(&lcd, "Hour: ");
	temp = num_returned();

	while ((temp < 0) || (temp >=24)){
		lcd_display_clear(&lcd);
		lcd_display_return_home(&lcd);
		lcd_send_command(&lcd, LCD_CMD_Don_nC_nBL);
		lcd_print_string(&lcd, "Not valid!");
		HAL_Delay(2000);
		lcd_display_clear(&lcd);
		lcd_display_return_home(&lcd);
		lcd_print_string(&lcd, "Hours: ");
		temp = num_returned();
	}

	current_time.hours = temp;
	temp = 0;

	lcd_display_clear(&lcd);
	lcd_display_return_home(&lcd);

	lcd_print_string(&lcd, "Minutes: ");
	temp = num_returned();

	while ((temp < 0) || (temp >=60)){
		lcd_display_clear(&lcd);
		lcd_display_return_home(&lcd);
		lcd_send_command(&lcd, LCD_CMD_Don_nC_nBL);
		lcd_print_string(&lcd, "Not valid!");
		HAL_Delay(2000);
		lcd_display_clear(&lcd);
		lcd_display_return_home(&lcd);
		lcd_print_string(&lcd, "Minutes: ");
		temp = num_returned();
	}

	current_time.minutes = temp;
	temp = 0;

	lcd_display_clear(&lcd);
	lcd_display_return_home(&lcd);

	lcd_print_string(&lcd, "Seconds: ");
	temp = num_returned();

	while ((temp < 0) || (temp >=60)){
		lcd_display_clear(&lcd);
		lcd_display_return_home(&lcd);
		lcd_send_command(&lcd, LCD_CMD_Don_nC_nBL);
		lcd_print_string(&lcd, "Not valid!");
		HAL_Delay(2000);
		lcd_display_clear(&lcd);
		lcd_display_return_home(&lcd);
		lcd_print_string(&lcd, "Seconds: ");
		temp = num_returned();
	}

	current_time.seconds = temp;
	temp = 0;

	lcd_display_clear(&lcd);
	lcd_display_return_home(&lcd);
	lcd_send_command(&lcd, LCD_CMD_Don_nC_nBL);

	rtc_set_current_time(&rtc, &current_time);
	rtc_set_current_date(&rtc, &current_date);

}

/**
 * @fn void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef*)
 * @brief Callback function triggered after TIMER6 issues an interrupt.
 * It will get the date and time every second and print them on the LCD screen.
 * @param htim Timer handler
 * @retval none
 */

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
	RTC_time_t current_time;
	RTC_date_t current_date;

	lcd_set_cursor(&lcd, 1,5);

	rtc_get_current_time(&rtc, &current_time);
	rtc_get_current_date(&rtc, &current_date);

	lcd_print_string(&lcd, time_to_string(&current_time));

	lcd_set_cursor(&lcd, 2,1);
	lcd_print_string(&lcd, date_to_string(&current_date));
	lcd_send_command(&lcd, LCD_CMD_MCR);
	lcd_print_string(&lcd, "<");
	lcd_print_string(&lcd, DAY_OF_WEEK(current_date.day));
	lcd_print_string(&lcd, ">");
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
  MX_I2C1_Init();
  MX_I2C2_Init();
  MX_TIM6_Init();
  /* USER CODE BEGIN 2 */

  // Initialization of external modules
  keypad = keypad_init();
  lcd_init_I2C(&lcd, &hi2c1);
  if (rtc_init(&rtc, &hi2c2)){
	printf("RTC init has failed\n");
	while(1);
  }

  // Initialising lcd
  lcd_set_cursor(&lcd, 1, 3);
  lcd_send_command(&lcd, LCD_CMD_Don_nC_nBL);
  lcd_print_string(&lcd, "Initialising");

  HAL_Delay(2000);

  config_menu();

  HAL_TIM_Base_Start_IT(&htim6);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief I2C2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C2_Init(void)
{

  /* USER CODE BEGIN I2C2_Init 0 */

  /* USER CODE END I2C2_Init 0 */

  /* USER CODE BEGIN I2C2_Init 1 */

  /* USER CODE END I2C2_Init 1 */
  hi2c2.Instance = I2C2;
  hi2c2.Init.ClockSpeed = 100000;
  hi2c2.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c2.Init.OwnAddress1 = 0;
  hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c2.Init.OwnAddress2 = 0;
  hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C2_Init 2 */

  /* USER CODE END I2C2_Init 2 */

}

/**
  * @brief TIM6 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM6_Init(void)
{

  /* USER CODE BEGIN TIM6_Init 0 */

  /* USER CODE END TIM6_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM6_Init 1 */

  /* USER CODE END TIM6_Init 1 */
  htim6.Instance = TIM6;
  htim6.Init.Prescaler = 255;
  htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim6.Init.Period = 62499;
  htim6.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim6) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim6, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM6_Init 2 */

  /* USER CODE END TIM6_Init 2 */

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
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, GPIO_PIN_RESET);

  /*Configure GPIO pin : PA0 */
  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PD8 PD9 PD10 PD11 */
  GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pins : PD0 PD1 PD2 PD3 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI0_IRQn, 14, 0);
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/**
 * @fn void HAL_GPIO_EXTI_Callback(uint16_t)
 * @brief Callback function triggered after the user press the "User button".
 * @param GPIO_Pin Specifies the pins connected EXTI line.
 */

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	config_menu();
}

/**
 * @fn Keypad_HandleTypeDef keypad_init(void)
 * @brief Initialise the keypad handler by setting which GPIO pins and ports are used to work with the Keypad.
 * @return Keypad_HandleTypeDef Keypad handler.
 */

Keypad_HandleTypeDef keypad_init(void){

    Keypad_HandleTypeDef keypad;

    Keypad_PinDef pin_columns[4] = {COLUMN_1_Pin, COLUMN_2_Pin, COLUMN_3_Pin, COLUMN_4_Pin};
    Keypad_PortDef port_columns[4] = { COLUMN_1_GPIO_Port, COLUMN_2_GPIO_Port, COLUMN_3_GPIO_Port, COLUMN_4_GPIO_Port };

    Keypad_PinDef pin_rows[4] = {ROW_1_Pin, ROW_2_Pin, ROW_3_Pin, ROW_4_Pin};
    Keypad_PortDef port_rows[4] = { ROW_1_GPIO_Port, ROW_2_GPIO_Port, ROW_3_GPIO_Port, ROW_4_GPIO_Port };

    keypad = keypad_create(pin_rows, port_rows, pin_columns, port_columns);

    return keypad;

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
  __disable_irq();
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
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
