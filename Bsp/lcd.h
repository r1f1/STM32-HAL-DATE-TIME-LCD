/**
 ******************************************************************************
 * @file     lcd.h
 * @author   Rafael Hern├índez Marrero (rafaelh96@gmail.com)
 * @version  V1.0
 * @date     02/11/2023 20:00:00
 * @brief	 Functions declarations available to the user for working with the HD44780 LCD along with the I/O expander for I2C bus PCF8574
 ******************************************************************************
 */

#ifndef LCD_H_
#define LCD_H_

//***************************************** Includes ******************************************//

#include "stm32f4xx_hal.h"
#include <stdint.h>

//*************************************** Helper macros ***************************************//

#define INIT_3						0x3
#define INIT_2						0x2
#define RS_PIN_RESET				0
#define RS_PIN_SET					1

//*********************************** Display rows and columns *******************************//

#define DISPLAY_ROW_1				1
#define DISPLAY_ROW_2				2
#define DISPLAY_COLUMN_1			1
#define DISPLAY_COLUMN_2			2
#define DISPLAY_COLUMN_3			3
#define DISPLAY_COLUMN_4			4
#define DISPLAY_COLUMN_5			5
#define DISPLAY_COLUMN_6			6
#define DISPLAY_COLUMN_7			7
#define DISPLAY_COLUMN_8			8
#define DISPLAY_COLUMN_9			9
#define DISPLAY_COLUMN_10			10
#define DISPLAY_COLUMN_11			11
#define DISPLAY_COLUMN_12			12
#define DISPLAY_COLUMN_13			13
#define DISPLAY_COLUMN_14			14
#define DISPLAY_COLUMN_15			15
#define DISPLAY_COLUMN_16			16


//***************************************** LCD types *****************************************//

typedef struct{

	I2C_HandleTypeDef 	*I2C_Handle;
	uint32_t 			I2C_Addr;

}LCD_HandleTypeDef;

//***************************************** Pin index *****************************************//

#define LCD_IO_RS	   				0
#define LCD_IO_RW	   				1
#define LCD_IO_EN	   				2
#define LCD_IO_BL					3
#define LCD_IO_D4	   				4
#define LCD_IO_D5	   				5
#define LCD_IO_D6	   				6
#define LCD_IO_D7	   				7

//*********************************** PCF8574 slave address ************************************//

#define SLAVE_ADDRESS_PCF			0x27<<1


//*************************************** LCD Commands ****************************************//

#define LCD_CMD_CLEAR 				0x01 			// Clear display
#define LCD_CMD_HOME 				0x02			// Return home

/* Entry mode set */

#define LCD_CMD_CD_nDS 				0x04 			// Cursor decrement and display doesn't shift
#define LCD_CMD_CD_DSR 				0x05 			// Cursor decrement and display shifts to the right
#define LCD_CMD_CI_nDS 				0x06 			// Cursor increment and display doesn't shift
#define LCD_CMD_CI_DSL 				0x07 			// Cursor increment and display shifts to the left

/* Display on/off control */

#define LCD_CMD_Doff_nC_nBL 		0x08 			// Turn off display (no characters are shown), cursor is not displayed and the character box is not blinking
#define LCD_CMD_Doff_nC_BL 			0x09 			// Turn off display (no characters are shown), cursor is not displayed and the character box is blinking
#define LCD_CMD_Doff_C_nBL 			0x0A 			// Turn off display (no characters are shown), cursor is displayed and the character box is not blinking
#define LCD_CMD_Doff_C_BL 			0x0B 			// Turn off display (no characters are shown), cursor is displayed and the character box is blinking
#define LCD_CMD_Don_nC_nBL 			0x0C 			// Turn on display (characters are shown), cursor is not displayed and the character box is not blinking
#define LCD_CMD_Don_nC_BL 			0x0D 			// Turn on display (characters are shown), cursor is not displayed and the character box is blinking
#define LCD_CMD_Don_C_nBL 			0x0E 			// Turn on display (characters are shown), cursor is displayed and the character box is not blinking
#define LCD_CMD_Don_C_BL 			0x0F 			// Turn on display (characters are shown), cursor is displayed and the character box is blinking

/* Cursor or display shift */

#define LCD_CMD_MCL 				0x10 			// Move cursor to the left
#define LCD_CMD_MCR 				0x14 			// Move cursor to the right
#define LCD_CMD_MDL 				0x18 			// Move display to the left
#define LCD_CMD_MDR 				0x1C 			// Move display to the right

/* Function set */

#define LCD_CMD_4DL_1L_5x8D 		0x20 			// 4-bits data length, 1 line displayed and 5x8 dots character font
#define LCD_CMD_4DL_1L_5x10D 		0x24 			// 4-bits data length, 1 line displayed and 5x10 dots character font
#define LCD_CMD_4DL_2L_5x8D 		0x28 			// 4-bits data length, 2 lines displayed and 5x8 dots character font
#define LCD_CMD_4DL_2L_5x10D 		0x2C 			// 4-bits data length, 2 lines displayed and 5x10 dots character font
#define LCD_CMD_5DL_1L_5x8D 		0x30 			// 5-bits data length, 1 line displayed and 5x8 dots character font
#define LCD_CMD_5DL_1L_5x10D 		0x34 			// 5-bits data length, 1 line displayed and 5x10 dots character font
#define LCD_CMD_5DL_2L_5x8D 		0x38 			// 5-bits data length, 2 lines displayed and 5x8 dots character font
#define LCD_CMD_5DL_2L_5x10D 		0x3C 			// 5-bits data length, 2 lines displayed and 5x10 dots character font


//************************************** Function declarations ****************************************//

void lcd_init_I2C(LCD_HandleTypeDef *, I2C_HandleTypeDef *);
void lcd_init(LCD_HandleTypeDef *);
void lcd_send_command(LCD_HandleTypeDef *, uint8_t cmd);
void lcd_print_char(LCD_HandleTypeDef *, uint8_t data);
void lcd_display_clear(LCD_HandleTypeDef *);
void lcd_display_return_home(LCD_HandleTypeDef *);
void lcd_print_string(LCD_HandleTypeDef *, char*);
void lcd_set_cursor(LCD_HandleTypeDef *, uint8_t row, uint8_t column);


#endif /* LCD_H_ */
