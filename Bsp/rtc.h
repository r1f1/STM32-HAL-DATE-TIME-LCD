/**
 ******************************************************************************
 * @file     rtc.h
 * @author   Rafael Hernández Marrero (rafaelh96@gmail.com)
 * @version  V1.0
 * @date     03/11/2023 12:04:00
 * @brief	 Functions declarations available to the user for working with the DS1307 rtc module.
 ******************************************************************************
 */

#ifndef RTC_H_
#define RTC_H_

//******************************************** Includes ********************************************//

#include "stm32f4xx_hal.h"

//*************************************** Register addresses ***************************************//
#define RTC_ADDR_SEC 			0x00
#define RTC_ADDR_MIN 			0x01
#define RTC_ADDR_HRS			0x02
#define RTC_ADDR_DAY			0x03
#define RTC_ADDR_DATE			0x04
#define RTC_ADDR_MONTH			0x05
#define RTC_ADDR_YEAR			0x06

//****************************************** Time format ******************************************//

#define TIME_FORMAT_12HRS_AM 	0
#define TIME_FORMAT_12HRS_PM 	1
#define TIME_FORMAT_24HRS 		2

//*************************************** DS1307 slave address *************************************//

#define RTC_I2C_ADDRESS      	0x68<<1

//****************************************** Helper macros *****************************************//

#define DAY_OF_WEEK(x)		    ( (x == 1) ? "Mon":\
							     (x == 2) ? "Tue":\
							     (x == 3) ? "Wed":\
							     (x == 4) ? "Thu":\
							     (x == 5) ? "Fri":\
							     (x == 6) ? "Sat":\
							     (x == 7) ? "Sun":0 )

#define MONDAY  				1
#define TUESDAY  				2
#define WEDNESDAY   			3
#define THURSDAY  				4
#define FRIDAY  				5
#define SATURDAY  				6
#define SUNDAY 					7

//***************************************** RTC types *****************************************//

/**
 * @struct RTC_date_t rtc.h "rtc.h"
 * @brief Date struct members.
 *
 */

typedef struct
{
	uint8_t month;
	uint8_t year;
	uint8_t date;
	uint8_t day;
}RTC_date_t;

/**
 * @struct RTC_time_t rtc.h "rtc.h"
 * @brief Time struct members.
 *
 */
typedef struct
{
	uint8_t seconds;
	uint8_t minutes;
	uint8_t hours;
	uint8_t time_format;
}RTC_time_t;

/**
 * @struct RTC_HandleTypeDef rtc.h "rtc.h"
 * @brief I2C handle
 *
 */
typedef struct{

	I2C_HandleTypeDef 	*I2C_Handle;
	uint32_t 			I2C_Addr;

}RTC_HandleTypeDef;


//************************************* Function declarations ***********************************//

uint8_t rtc_init(RTC_HandleTypeDef *, I2C_HandleTypeDef *);

void rtc_set_current_time(RTC_HandleTypeDef *, RTC_time_t *);
void rtc_get_current_time(RTC_HandleTypeDef *, RTC_time_t *);

void rtc_set_current_date(RTC_HandleTypeDef *, RTC_date_t *);
void rtc_get_current_date(RTC_HandleTypeDef *, RTC_date_t *);

#endif /* RTC_H_ */
