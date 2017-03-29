/*
 * oled.h
 *
 *  Created on: 14 styczen 2016
 *      Author: Przemek
 */

#ifndef OLED_H_
#define OLED_H_

#include "stm32f0xx.h"
#include "DMB/rejestry.h"
#define DMA_ON 0

/* DC */
#define LCD_DC_GPIO 	GPIOA
#define LCD_DC_PIN 		1

#define LCD_DC_OUT 		GPIO_MODER(LCD_DC_GPIO) |= ( 1 << ( 2 * LCD_DC_PIN))
#define LCD_DC_SET 		GPIO_BSRR(LCD_DC_GPIO) = (1 << LCD_DC_PIN);
#define LCD_DC_CLR		GPIO_BRR(LCD_DC_GPIO) = (1 << LCD_DC_PIN);

/* CE */
#define CE_USE 1

#define LCD_CE_GPIO 	GPIOA
#define LCD_CE_PIN 		2
#define LCD_CE_OUT 		GPIO_MODER(LCD_CE_GPIO) |= ( 1 << ( 2 * LCD_CE_PIN))
#define LCD_CE_SET 		GPIO_BSRR(LCD_CE_GPIO) = (1 << LCD_CE_PIN);
#define LCD_CE_CLR		GPIO_BRR(LCD_CE_GPIO) = (1 << LCD_CE_PIN);

/* RST */
#define LCD_RST_GPIO	GPIOA
#define LCD_RST_PIN 	3
#define LCD_RST_OUT 	GPIO_MODER(LCD_RST_GPIO) |= ( 1 << ( 2 * LCD_RST_PIN))
#define LCD_RST_SET 	GPIO_BSRR(LCD_RST_GPIO) = (1 << LCD_RST_PIN);
#define LCD_RST_CLR		GPIO_BRR(LCD_RST_GPIO) = (1 << LCD_RST_PIN);


#define LCD_CMD     0
#define LCD_DATA    1

#define USE_BUFOR 1

#define SSD1306_WITCH 128

#define REFRESH_MIN 0x80
#define REFRESH_MID 0xB0
#define REFRESH_MAX 0xf0

#define SSD1306_SETCONTRAST 0x81
#define SSD1306_DISPLAYALLON_RESUME 0xA4
#define SSD1306_DISPLAYALLON 0xA5
#define SSD1306_NORMALDISPLAY 0xA6
#define SSD1306_INVERTDISPLAY 0xA7
#define SSD1306_DISPLAYOFF 0xAE
#define SSD1306_DISPLAYON 0xAF

#define SSD1306_SETDISPLAYOFFSET 0xD3
#define SSD1306_SETCOMPINS 0xDA

#define SSD1306_SETVCOMDETECT 0xDB

#define SSD1306_SETDISPLAYCLOCKDIV 0xD5
#define SSD1306_SETPRECHARGE 0xD9

#define SSD1306_SETMULTIPLEX 0xA8

#define SSD1306_SETLOWCOLUMN 0x00
#define SSD1306_SETHIGHCOLUMN 0x10

#define SSD1306_SETSTARTLINE 0x40

#define SSD1306_MEMORYMODE 0x20
#define SSD1306_COLUMNADDR 0x21
#define SSD1306_PAGEADDR   0x22

#define SSD1306_COMSCANINC 0xC0
#define SSD1306_COMSCANDEC 0xC8

#define SSD1306_SEGREMAP 0xA0

#define SSD1306_CHARGEPUMP 0x8D

#define SSD1306_EXTERNALVCC 0x1
#define SSD1306_SWITCHCAPVCC 0x2

// Scrolling #defines
#define SSD1306_ACTIVATE_SCROLL 0x2F
#define SSD1306_DEACTIVATE_SCROLL 0x2E
#define SSD1306_SET_VERTICAL_SCROLL_AREA 0xA3
#define SSD1306_RIGHT_HORIZONTAL_SCROLL 0x26
#define SSD1306_LEFT_HORIZONTAL_SCROLL 0x27
#define SSD1306_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL 0x29
#define SSD1306_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL 0x2A

void oled_init(void);
void init_spi(void);
void oled_wyslij_bajt(uint8_t c_d, uint8_t data);
void oled_display();
void oled_invert(uint8_t kolor);
void oled_set_pixel(int8_t x, int8_t y, uint8_t pixel);
void oled_narysuj_znak(uint8_t x, uint8_t y,char c, uint8_t kolor, uint8_t tlo, uint8_t rozmiar);
void oled_odswiez_obszar(uint8_t strona, uint8_t cnt, uint8_t kolumna_startowa, uint8_t kolumna_koncowa);
void oled_ustaw_kontrast(uint8_t kontrast);

// kierunek = 0 dla przewijania w prawo, 1 dla w lewo
void oled_scroll_boki_ustawienia(uint8_t kierunek, uint8_t start, uint8_t predkosc, uint8_t koniec);
void oled_scroll_pion_ustawienia(uint8_t kierunek, uint8_t start, uint8_t predkosc, uint8_t koniec, uint8_t vertical_ofset);
void oled_scroll_wl_wyl(uint8_t stan);

#endif /* OLED_H_ */
