/*
 * lcd_uc1701.h
 *
 *  Created on: 2017-03-17
 *      Author: Przemek
 */

#ifndef LCD_UC1701_H_
#define LCD_UC1701_H_

#include "stm32f0xx.h"
#include "DMB/rejestry.h"

#define LCD_X 128
#define LCD_Y 64

#define LCD_WIDTH 128

// komendy czy polecenia
#define lcd_uc1701_komenda 0
#define lcd_uc1701_dane 1

#define LCD_DC_GPIO 	GPIOA
#define LCD_DC_PIN 		1
#define LCD_DC_OUT 		GPIO_MODER(LCD_DC_GPIO) |= ( 1 << ( 2 * LCD_DC_PIN))
#define LCD_DC_SPEED	GPIO_OSPEEDR(LCD_DC_GPIO) |= (1 << ( LCD_DC_PIN * 2 ) ) | ( 1 << ( ( LCD_DC_PIN * 2 ) + 1));
#define LCD_DC_SET 		GPIO_BSRR(LCD_DC_GPIO) = (1 << LCD_DC_PIN);
#define LCD_DC_CLR		GPIO_BRR(LCD_DC_GPIO) = (1 << LCD_DC_PIN);

#define LCD_CE_GPIO 	GPIOA
#define LCD_CE_PIN 		2
#define LCD_CE_OUT 		GPIO_MODER(LCD_CE_GPIO) |= ( 1 << ( 2 * LCD_CE_PIN))
#define LCD_CE_SPEED	GPIO_OSPEEDR(LCD_CE_GPIO) |= (1 << ( LCD_CE_PIN * 2 ) ) | ( 1 << ( ( LCD_CE_PIN * 2 ) + 1));
#define LCD_CE_SET 		GPIO_BSRR(LCD_CE_GPIO) = (1 << LCD_CE_PIN);
#define LCD_CE_CLR		GPIO_BRR(LCD_CE_GPIO) = (1 << LCD_CE_PIN);

#define LCD_RST_GPIO	GPIOA
#define LCD_RST_PIN 	3
#define LCD_RST_OUT 	GPIO_MODER(LCD_RST_GPIO) |= ( 1 << ( 2 * LCD_RST_PIN))
#define LCD_RST_SPEED	GPIO_OSPEEDR(LCD_RST_GPIO) |= (1 << ( LCD_RST_PIN * 2 ) ) | ( 1 << ( ( LCD_RST_PIN * 2 ) + 1));
#define LCD_RST_SET 	GPIO_BSRR(LCD_RST_GPIO) = (1 << LCD_RST_PIN);
#define LCD_RST_CLR		GPIO_BRR(LCD_RST_GPIO) = (1 << LCD_RST_PIN);

// funkcje podstawowe dla wyswietlacza
void lcd_uc1701_init(void);
void lcd_uc1701_wyslij_bajt( unsigned char, unsigned char );
void lcd_uc1701_czysc_ekran(void);
void lcd_uc1701_negatyw(uint8_t);

// funkcja wybierajaca linie do ktorej przesylamy dane
void lcd_uc1701_ustaw_linie(uint8_t);

// funkcje operujace na buforze
void lcd_uc1701_buf_wyswietl(void);


#endif /* LCD_UC1701_H_ */
