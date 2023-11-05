
/**
 ******************************************************************************
 * @file     rtc.c
 * @author   Rafael Hernández Marrero (rafaelh96@gmail.com)
 * @version  V1.0
 * @date     03/11/2023 12:04:00
 * @brief	 Functions definitions available to the user for working with the DS1307 rtc module.
 ******************************************************************************
 */

//******************************************** Includes ********************************************//

#include<stdint.h>
#include "rtc.h"

//**************************************** Static declarations *************************************//

static uint8_t rtc_read(RTC_HandleTypeDef *, uint8_t);
static void rtc_write(RTC_HandleTypeDef *, uint8_t value,uint8_t);
static uint8_t bcd_to_binary(uint8_t);
static uint8_t binary_to_bcd(uint8_t);

//************************************** Function definitions **************************************//


/**
 * @fn uint8_t rtc_init(RTC_HandleTypeDef*, I2C_HandleTypeDef*)
 * @brief This function initialise the DS1307 module by setting the clock halt (CH) bit to 0.
 * @param rtc RTC handler
 * @param I2C_Handle I2C handler.
 * @return uint8_t If the clock halt (CH) bit returns 1 the module is disable, if it returns 0 is enable.
 */

uint8_t rtc_init(RTC_HandleTypeDef *rtc, I2C_HandleTypeDef *I2C_Handle)
{

	rtc->I2C_Addr = RTC_I2C_ADDRESS;
	rtc->I2C_Handle = I2C_Handle;

	//1. Make clock halt = 0;
	rtc_write(rtc, 0x00,RTC_ADDR_SEC);

	//2. Read back clock halt bit
	uint8_t clock_state = rtc_read(rtc, RTC_ADDR_SEC);

	return ((clock_state >> 7 ) & 0x1);

}



/**
 * @fn void rtc_set_current_time(RTC_HandleTypeDef*, RTC_time_t*)
 * @brief This function set the time introduced by the user differentiating between the 12 and 24 hours format.
 * @brief Each parameter have their own address and they are set in BCD format.
 * @param rtc RTC handler.
 * @param rtc_time Time handler.
 */
void rtc_set_current_time(RTC_HandleTypeDef *rtc, RTC_time_t *rtc_time)
{

	uint8_t seconds, minutes, hours;
	seconds = binary_to_bcd(rtc_time->seconds);
	minutes = binary_to_bcd(rtc_time->minutes);
	hours = binary_to_bcd(rtc_time->hours);

	seconds &= ~(1 << 7);

	rtc_write(rtc, seconds, RTC_ADDR_SEC);

	rtc_write(rtc, minutes, RTC_ADDR_MIN);

	if (rtc_time->time_format == TIME_FORMAT_24HRS){
		hours &= ~(1 << 6);
	}else{
		hours |= (1 << 6);
		hours = (rtc_time->time_format == TIME_FORMAT_12HRS_PM) ? hours | (1 << 5) : hours & ~(1 << 5);
	}

	rtc_write(rtc, hours, RTC_ADDR_HRS);

}

/**
 * @fn void rtc_set_current_date(RTC_HandleTypeDef*, RTC_date_t*)
 * @brief This function set the date introduced by the user.
 * @brief Each parameter have their own address and they are set in BCD format.
 * @param rtc RTC handler.
 * @param rtc_date Date handler.
 */

void rtc_set_current_date(RTC_HandleTypeDef *rtc, RTC_date_t *rtc_date)
{

	rtc_write(rtc, binary_to_bcd(rtc_date->month),RTC_ADDR_MONTH);

	rtc_write(rtc, binary_to_bcd(rtc_date->year),RTC_ADDR_YEAR);

	rtc_write(rtc, binary_to_bcd(rtc_date->date),RTC_ADDR_DATE);

	rtc_write(rtc, binary_to_bcd(rtc_date->day), RTC_ADDR_DAY);

}

/**
 * @fn void rtc_get_current_time(RTC_HandleTypeDef*, RTC_time_t*)
 * @brief This function get the current date.
 * @brief The returned values are set back to binary format.
 * @param rtc RTC handler.
 * @param rtc_time Time handler.
 */
void rtc_get_current_time(RTC_HandleTypeDef *rtc, RTC_time_t *rtc_time)
{

	uint8_t seconds, minutes, hours;

	seconds = rtc_read(rtc, RTC_ADDR_SEC);
	minutes = rtc_read(rtc, RTC_ADDR_MIN);
	hours = rtc_read(rtc, RTC_ADDR_HRS);

	seconds &= ~(1 << 7);

	rtc_time->seconds = bcd_to_binary(seconds);
	rtc_time->minutes = bcd_to_binary(minutes);

	if(hours & (1 << 6)){										// 12 hours format
		rtc_time->time_format = !( (hours & (1 << 5)) == 0 );
		hours &= ~(0x3 << 5);
	}else {
		rtc_time->time_format = TIME_FORMAT_24HRS;
	}

	rtc_time->hours = bcd_to_binary(hours);
}


/**
 * @fn void rtc_get_current_date(RTC_HandleTypeDef*, RTC_date_t*)
 * @brief This function get the current date.
 * @brief The returned values are set back to binary format.
 * @param rtc RTC handler.
 * @param rtc_date Date handler.
 */
void rtc_get_current_date(RTC_HandleTypeDef *rtc, RTC_date_t *rtc_date)
{
	rtc_date->date =  bcd_to_binary(rtc_read(rtc, RTC_ADDR_DATE));
	rtc_date->month = bcd_to_binary(rtc_read(rtc, RTC_ADDR_MONTH));
	rtc_date->year = bcd_to_binary(rtc_read(rtc, RTC_ADDR_YEAR));
	rtc_date->day = bcd_to_binary(rtc_read(rtc, RTC_ADDR_DAY));

}

/**
 * @fn void rtc_write(RTC_HandleTypeDef*, uint8_t, uint8_t)
 * @brief This function sends a value to the corresponding address.
 * @param rtc RTC handler.
 * @param value Any value within the Time or Date handler.
 * @param reg_addr Address for the Time or Date handler values.
 */
static void rtc_write(RTC_HandleTypeDef *rtc, uint8_t value, uint8_t reg_addr)
{
	uint8_t tx[2];
	tx[0] = reg_addr;
	tx[1] = value;
	HAL_I2C_Master_Transmit(rtc->I2C_Handle, rtc->I2C_Addr, tx, 2, 100);
}

/**
 * @fn uint8_t rtc_read(RTC_HandleTypeDef*, uint8_t)
 * @brief This function reads a value from the corresponding address.
 * @param rtc RTC handler.
 * @param reg_addr Address for the Time or Date handler values.
 * @return uint8_t Data returned after sending the corresponding address.
 */
static uint8_t rtc_read(RTC_HandleTypeDef *rtc, uint8_t reg_addr)
{
	uint8_t data;
    HAL_I2C_Master_Transmit(rtc->I2C_Handle, rtc->I2C_Addr, &reg_addr, 1, 100);
    HAL_I2C_Master_Receive(rtc->I2C_Handle, rtc->I2C_Addr, &data, 1, 100);

    return data;
}

/**
 * @fn uint8_t binary_to_bcd(uint8_t)
 * @brief This function converts binary data into its corresponding binary-code decimal format (BCD).
 * @param value Value in binary format.
 * @return uint8_t Data returned in BCD format.
 */

static uint8_t binary_to_bcd(uint8_t value)
{
	uint8_t m , n;
	uint8_t bcd;

	bcd = value;
	if(value >= 10)
	{
		m = value /10;
		n = value % 10;
		bcd = (m << 4) | n ;
	}

	return bcd;
}

/**
 * @fn uint8_t bcd_to_binary(uint8_t)
 * @brief This function sets back the value to binary format.
 * @param value Value in BCD format.
 * @return uint8_t Data returned in binary format.
 */
static uint8_t bcd_to_binary(uint8_t value)
{
	uint8_t m , n;
	m = (uint8_t) ((value >> 4 ) * 10);
	n =  value & (uint8_t)0x0F;
	return (m+n);
}

