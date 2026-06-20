/*
 * lcd.h
 *
 *  Created on: Dec 6, 2024
 *      Author: saide
 */

#ifndef INC_LCD_H_
#define INC_LCD_H_

void HAL_LCD_STRING(char str[]);
void WRITE_HIGH_NIBBLE(unsigned char a);
void WRITE_LOW_NIBBLE(unsigned char a);
void HAL_LCD_WRITE_COMMAND(unsigned char a);
void HAL_LCD_WRITE_DATA(unsigned char a);
void HAL_LCD_Init(void);
void HAL_LCD_STRING(char str[]);

#endif /* INC_LCD_H_ */
