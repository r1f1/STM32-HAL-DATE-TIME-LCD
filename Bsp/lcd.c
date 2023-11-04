
/**
 ******************************************************************************
 * @file     lcd.c
 * @author   Rafael Hernández Marrero (rafaelh96@gmail.com)
 * @version  V1.0
 * @date     02/11/2023 20:00:00
 * @brief	 Functions definitions available to the user for working with the HD44780 LCD along with the I/O expander for I2C bus PCF8574
 ******************************************************************************
 */

//******************************************** Includes ********************************************//

#include "lcd.h"
#include "stm32f4xx_hal.H"

//**************************************** Static declarations *************************************//

static void udelay(uint32_t cnt);
static void lcd_write(LCD_HandleTypeDef *, uint8_t, uint8_t);


//************************************** Function definitions **************************************//

/*************************************************************************************
 * @fn									- lcd_send_command
 *
 * @brief								- This function splits the command value into two nibbles and sends the higher nibble first, followed by the lower nibble.
 *										- Rs pin is set to 0.
 *
 * @param[in]  lcd						- LCD handler.
 *
 * @param[in]  cmd						- Command.
 */

void lcd_send_command(LCD_HandleTypeDef *lcd, uint8_t cmd)
{

	lcd_write(lcd, (cmd >> 4), RS_PIN_RESET);
	lcd_write(lcd, (cmd & 0x0F), RS_PIN_RESET);

}

/*************************************************************************************
 * @fn									- lcd_print_char
 *
 * @brief								- This function splits the command value into two nibbles and sends the higher nibble first, followed by the lower nibble.
 *										- Rs pin is set to 1.
 *
 * @param[in]  lcd						- LCD handler.
 *
 * @param[in]  data						- Character to send.
 */

void lcd_print_char(LCD_HandleTypeDef *lcd, uint8_t data)
{

	lcd_write(lcd, (data >> 4), RS_PIN_SET);  			//Higher nibble
	lcd_write(lcd, (data & 0x0F), RS_PIN_SET); 			//Lower nibble

}

/*************************************************************************************
 * @fn									- lcd_print_string
 *
 * @brief								- This function iterates the message character by character.
 *
 * @param[in]  lcd						- LCD handler.
 *
 * @param[in]  message					- String to send.
 */

void lcd_print_string(LCD_HandleTypeDef *lcd, char *message)
{

      do
      {
          lcd_print_char(lcd, (uint8_t)*message++);
      }
      while (*message != '\0');

}

/*************************************************************************************
 * @fn									- lcd_write
 *
 * @brief								- This function sends the nibble to the LCD screen over I2C bus.
 *
 * @param[in]  lcd						- LCD handler.
 *
 * @param[in]  nibble					- 4-bits to send.
 *
 * @param[in]  Rs						- Rs pin.
 */

void lcd_write(LCD_HandleTypeDef *lcd, uint8_t nibble, uint8_t Rs){

	uint8_t Data = (nibble << LCD_IO_D4);
	Data |= Rs << LCD_IO_RS;
	Data |= (1 << LCD_IO_BL); 						// Backlight always has to be at 1
	Data |= (1 << LCD_IO_EN);

	HAL_I2C_Master_Transmit(lcd->I2C_Handle, lcd->I2C_Addr, &Data, 1, 100);
	HAL_Delay(1);
	Data &= ~(1 << LCD_IO_EN);

	HAL_I2C_Master_Transmit(lcd->I2C_Handle, lcd->I2C_Addr, &Data, 1, 100);

}

/*************************************************************************************
 * @fn									- lcd_init_I2C
 *
 * @brief								- This function initialise the LCD handler with the I2C related data.
 *
 * @param[in]  lcd						- LCD handler.
 *
 * @param[in]  I2C_Handle				- I2C handler.
 */

void lcd_init_I2C(LCD_HandleTypeDef *lcd, I2C_HandleTypeDef *I2C_Handle){

	lcd->I2C_Addr = SLAVE_ADDRESS_PCF;
	lcd->I2C_Handle = I2C_Handle;

	lcd_init(lcd);
}

/*************************************************************************************
 * @fn									- lcd_init
 *
 * @brief								- This function initialise the LCD screen following the steps recommended by the manufacturer.
 *
 * @param[in]  lcd						- LCD handler.
 */

void lcd_init(LCD_HandleTypeDef *lcd)
{

	HAL_Delay(50);

	// Initialise data at 0
	uint8_t data = 0;
	HAL_I2C_Master_Transmit(lcd->I2C_Handle, lcd->I2C_Addr, &data, 1, 100);

	HAL_Delay(40);

	// Send first nibble
	lcd_write(lcd, INIT_3, RS_PIN_RESET);

	HAL_Delay(5);

	// Send second nibble
	lcd_write(lcd, INIT_3, RS_PIN_RESET);

	udelay(100);

	// Send third and fourth nibble
	lcd_write(lcd, INIT_3, RS_PIN_RESET);
	lcd_write(lcd, INIT_2, RS_PIN_RESET);

	// Set 4-bits data length, display 2 lines and set 5x8 dots character font
	lcd_send_command(lcd, LCD_CMD_4DL_2L_5x8D);

	// Set display on, cursor on and character box not blinking
	lcd_send_command(lcd, LCD_CMD_Don_C_nBL);

	lcd_display_clear(lcd);

	// Entry mode set. Cursor increment and no display shifted
	lcd_send_command(lcd, LCD_CMD_CI_nDS);

}

/*************************************************************************************
 * @fn									- lcd_display_clear
 *
 * @brief								- This function clear the display.
 *
 * @param[in]  lcd						- LCD handler.
 */

void lcd_display_clear(LCD_HandleTypeDef *lcd)
{

	lcd_send_command(lcd, LCD_CMD_CLEAR);

	HAL_Delay(2);
}

/*************************************************************************************
 * @fn									- lcd_display_return_home
 *
 * @brief								- This function returns the cursor to the initial position.
 *
 * @param[in]  lcd						- LCD handler.
 */


void lcd_display_return_home(LCD_HandleTypeDef *lcd)
{

	lcd_send_command(lcd, LCD_CMD_HOME);

	HAL_Delay(2);
}

/*************************************************************************************
 * @fn									- lcd_set_cursor
 *
 * @brief								- This function set cursor to a specified location given by row and column information.
 *
 * @param[in]  lcd						- LCD handler.
 *
 * @param[in]  row						- Row Number (1 to 2)
 *
 * @param[in]  column					- Column Number (1 to 16).  Assuming a 2x16 characters display.
 */

void lcd_set_cursor(LCD_HandleTypeDef *lcd, uint8_t row, uint8_t column)
{
  column--;
  switch (row)
  {
    case 1:
      /* Set cursor to 1st row address and add index*/
      lcd_send_command(lcd, (column |= 0x80));
      break;
    case 2:
      /* Set cursor to 2nd row address and add index*/
        lcd_send_command(lcd, (column |= 0xC0));
      break;
    default:
      break;
  }
}

/*************************************************************************************
 * @fn									- udelay
 *
 * @brief								- This function create a delay in microseconds.
 *
 * @param[in]  cnt						- Count in microseconds.
 */

static void udelay(uint32_t cnt)
{
	for(uint32_t i=0 ; i < (cnt * 1); i++);
}
