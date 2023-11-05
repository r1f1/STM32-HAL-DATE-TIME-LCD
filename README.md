# STM32-HAL-DATE-TIME-LCD

## Overview


The purpose of this project is to display the date and time on the LCD screen, as previously introduced by the user. To achieve this goal, we used a general 4x4 matrix keypad to input the data, the RTC DS1307 to set the date and time via the I2C bus, and the HD44780 controller along with the PCF8574 I2C I/O expander to display the different parameters.

Before beginning this project, it was necessary to develop the drivers for each component, which you can find in my other repositories.

In this application, every time the MCU is powered on, the program will prompt for the date and time. Once this is done, it will display the data on the LCD screen, updating the time every second. An additional button is provided if the user wants to change the values at any point.

This project was tested on the [STM32DISCOVERY](https://www.st.com/en/evaluation-tools/stm32f4discovery.html) board.

![](pic123.png)

## Documentation

A brief documentation of the functions used in the application are shown below. For detailed instructions, refer to the doxygen compliant documentation within the `main.c` file.

### Functions

| Name | Description |
|     :---:    |     ---      |
| `number_to_string`   |   Convert an integer to a string, adding a 0 to integers smaller than 10   | 
| `time_to_string`   |  Set the time in this format `hh:mm:ss` | 
| `date_to_string`   |  Set the date in this format: `dd/mm/yyyy`  | 
| `num_returned`   |  Return the entered number via the keypad  | 
| `select_day`   |  Let the user choose the day of the week  | 
| `config_menu`   |  Let the user choose the time and date parameters, as long as they are correct  | 
| `HAL_TIM_PeriodElapsedCallback`   |  Callback function triggered after the TIMER issues an interrupt every second | 
| `HAL_GPIO_EXTI_Callback`   |   Callback function triggered after the user press the _User Button_ | 
| `keypad_init`   |   Initialization of the keypad handler  | 