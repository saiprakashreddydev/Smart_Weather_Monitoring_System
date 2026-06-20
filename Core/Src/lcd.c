/*
 * lcd.c
 *
 *  Created on: Dec 6, 2024
 *      Author: saide
 */


/*
 * lcd.c
 *
 *  Created on: Dec 6, 2024
 *      Author: saide
 */
#include "main.h"
void WRITE_HIGH_NIBBLE(unsigned char a)
{
	a=a>>4;
	if(a&0x1)
		HAL_GPIO_WritePin(GPIOB, LCD_D0_Pin,GPIO_PIN_SET);
	else
		HAL_GPIO_WritePin(GPIOB, LCD_D0_Pin,GPIO_PIN_RESET);
	if(a>>1&0x1)
		HAL_GPIO_WritePin(GPIOB, LCD_D1_Pin,GPIO_PIN_SET);
	else
		HAL_GPIO_WritePin(GPIOB, LCD_D1_Pin,GPIO_PIN_RESET);
	if(a>>2&0x1)
		HAL_GPIO_WritePin(GPIOB, LCD_D2_Pin,GPIO_PIN_SET);
	else
		HAL_GPIO_WritePin(GPIOB, LCD_D2_Pin,GPIO_PIN_RESET);
	if(a>>3&0x1)
		HAL_GPIO_WritePin(GPIOB, LCD_D3_Pin,GPIO_PIN_SET);
	else
		HAL_GPIO_WritePin(GPIOB, LCD_D3_Pin,GPIO_PIN_RESET);


	HAL_GPIO_WritePin(GPIOB, EN_Pin,GPIO_PIN_SET);
	HAL_Delay(5);
	HAL_GPIO_WritePin(GPIOB, EN_Pin,GPIO_PIN_RESET);
}


void WRITE_LOW_NIBBLE(unsigned char a)
{
	a&=0x0F;
		if(a&0x1)
			HAL_GPIO_WritePin(GPIOB, LCD_D0_Pin,GPIO_PIN_SET);
		else
			HAL_GPIO_WritePin(GPIOB, LCD_D0_Pin,GPIO_PIN_RESET);
		if(a>>1&0x1)
			HAL_GPIO_WritePin(GPIOB, LCD_D1_Pin,GPIO_PIN_SET);
		else
			HAL_GPIO_WritePin(GPIOB, LCD_D1_Pin,GPIO_PIN_RESET);
		if(a>>2&0x1)
			HAL_GPIO_WritePin(GPIOB, LCD_D2_Pin,GPIO_PIN_SET);
		else
			HAL_GPIO_WritePin(GPIOB, LCD_D2_Pin,GPIO_PIN_RESET);
		if(a>>3&0x1)
			HAL_GPIO_WritePin(GPIOB, LCD_D3_Pin,GPIO_PIN_SET);
		else
			HAL_GPIO_WritePin(GPIOB, LCD_D3_Pin,GPIO_PIN_RESET);

		HAL_GPIO_WritePin(GPIOB, EN_Pin,GPIO_PIN_SET);
		HAL_Delay(5);
		HAL_GPIO_WritePin(GPIOB, EN_Pin,GPIO_PIN_RESET);
}

void HAL_LCD_WRITE_COMMAND(unsigned char a)
{
	HAL_GPIO_WritePin(GPIOB,RS_Pin,GPIO_PIN_RESET);
	WRITE_HIGH_NIBBLE(a);
	WRITE_LOW_NIBBLE(a);
}

void HAL_LCD_WRITE_DATA(unsigned char a)
{
	HAL_GPIO_WritePin(GPIOB,RS_Pin,GPIO_PIN_SET);
	WRITE_HIGH_NIBBLE(a);
	WRITE_LOW_NIBBLE(a);
}
void HAL_LCD_Init(void)
{
		HAL_Delay(5);
		HAL_LCD_WRITE_COMMAND(0x33);
		HAL_Delay(1);
		HAL_LCD_WRITE_COMMAND(0x32);
		HAL_LCD_WRITE_COMMAND(0x0C);
		HAL_LCD_WRITE_COMMAND(0x01);
}
void HAL_LCD_STRING(char str[])
{
	int i;
	for(i=0;str[i]!=0;i++)
	{
	HAL_LCD_WRITE_DATA(str[i]);
    HAL_Delay(1);
	}
}
