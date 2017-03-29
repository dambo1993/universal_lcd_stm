#include "stm32f0xx.h"
#include <stdio.h>
#include <stdlib.h>
#include "delay_systick/delay_systick.h"
#include <stdlib.h>

#include "lcd_uc1701.h"
#include "../lcd.h"

extern uint8_t lcd_buff[ 128 * 8 ];
extern const uint8_t ASCII[][5];

void lcd_uc1701_init(void)
{
	// ustawienie pinow
	// uruchomienie zegara dla portu A
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;

	// pin mosi
	// alternate function
	GPIOA->MODER |= GPIO_MODER_MODER5_1;
	// high speed
	GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR5_1 | GPIO_OSPEEDER_OSPEEDR5_0;

	// pin sck
	// alternate function
	GPIOA->MODER |= GPIO_MODER_MODER7_1;
	// high speed
	GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR7_1 | GPIO_OSPEEDER_OSPEEDR7_0;

	// pin rst
	LCD_RST_OUT;
	LCD_RST_SPEED;

	// pin ce
	LCD_CE_OUT;
	LCD_CE_SPEED;

	// pin dc
	LCD_DC_OUT;
	LCD_DC_SPEED;

	//przypisanie funkcji alternatywnych dla pinow
	// (niepotrzebne, bo wszedzie sa alternate function 0)

	// ustawienie SPI
	// wlaczenie zegara
	RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;

	// ustawienie predkosci taktowania zegara (domyslnie FPU/2)
//	SPI1->CR1 |= SPI_CR1_BR_0 | SPI_CR1_BR_1; // przez 16
//	SPI1->CR1 |= SPI_CR1_BR_0; // przez 4

	// softwarowy pin NSS
	SPI1->CR1 |= SPI_CR1_SSM;
	SPI1->CR1 |= SPI_CR1_SSI;

	// tryb master
	SPI1->CR1 |= SPI_CR1_MSTR;

	// tylko nadawanie
	SPI1->CR1 |= SPI_CR1_BIDIOE;

	// 8 bitow danych:
	SPI1->CR2 |= SPI_CR2_DS_2 | SPI_CR2_DS_1 | SPI_CR2_DS_0;

	// uruchomienie SPI
	SPI1->CR1 |= SPI_CR1_SPE;

	// proces inicjalizacji:
    LCD_CE_SET;


    LCD_RST_CLR;

    _delay_ms(10);

    LCD_RST_SET;
    _delay_ms(10);

    // Set the LCD parameters...
    lcd_uc1701_wyslij_bajt(lcd_uc1701_komenda,0xE2);  //System Reset
    lcd_uc1701_wyslij_bajt(lcd_uc1701_komenda,0x40); // Set display start line to 0
    lcd_uc1701_wyslij_bajt(lcd_uc1701_komenda,0xA1); //Set SEG Direction
    lcd_uc1701_wyslij_bajt(lcd_uc1701_komenda,0xC0); //Set COM Direction
    lcd_uc1701_wyslij_bajt(lcd_uc1701_komenda,0xA2); //Set Bias = 1/9
    lcd_uc1701_wyslij_bajt(lcd_uc1701_komenda,0x2C);  //Boost ON
    lcd_uc1701_wyslij_bajt(lcd_uc1701_komenda,0x2E); //Voltage Regular On
    lcd_uc1701_wyslij_bajt(lcd_uc1701_komenda,0x2F); //Voltage Follower On
    lcd_uc1701_wyslij_bajt(lcd_uc1701_komenda,0xF8); //Set booster ratio to
    lcd_uc1701_wyslij_bajt(lcd_uc1701_komenda,0x00); //4x
    lcd_uc1701_wyslij_bajt(lcd_uc1701_komenda,0x23); //Set Resistor Ratio = 3
    lcd_uc1701_wyslij_bajt(lcd_uc1701_komenda,0x81);
    lcd_uc1701_wyslij_bajt(lcd_uc1701_komenda,0x28); //Set Electronic Volume = 40
    lcd_uc1701_wyslij_bajt(lcd_uc1701_komenda,0xAC);//Set Static indicator off
    lcd_uc1701_wyslij_bajt(lcd_uc1701_komenda,0x00);
    lcd_uc1701_wyslij_bajt(lcd_uc1701_komenda,0XA6); // Disable inverse
    lcd_uc1701_wyslij_bajt(lcd_uc1701_komenda,0xAF); //Set Display Enable
	_delay_ms(10);
	lcd_uc1701_wyslij_bajt(lcd_uc1701_komenda,0xA5); //display all points
	_delay_ms(20);
	lcd_uc1701_wyslij_bajt(lcd_uc1701_komenda,0xA4); //normal display

	lcd_uc1701_czysc_ekran();
}

void lcd_uc1701_wyslij_bajt(unsigned char c_d, unsigned char data )
{
    LCD_CE_CLR;

    if(c_d)
    {
       LCD_DC_SET;
    }
    else
    {
       LCD_DC_CLR;
    }
	while( SPI1->SR & SPI_SR_BSY );
	*(uint8_t *)&SPI1->DR = (uint8_t)data;
	uint8_t x = 5;
	while(x--);

    LCD_CE_SET;
}


void lcd_uc1701_buf_wyswietl()
{
	uint16_t licznik = 0;
	for(uint8_t i = 0; i < 8; i++ )
	{
		lcd_uc1701_ustaw_linie( i );
		LCD_DC_SET;
		LCD_CE_CLR;
		for(uint8_t j = 0; j < LCD_WIDTH; j++ )
		{
			while( SPI1->SR & SPI_SR_BSY );
			*(uint8_t *)&SPI1->DR = (uint8_t)lcd_buff[licznik++];
			uint8_t x = 5;
			while(x--);
		}
		LCD_CE_SET;
	}
}

// czysci ekran
void lcd_uc1701_czysc_ekran(void)
{
	for(uint8_t i = 0; i < 8; i++ )
	{
		lcd_uc1701_ustaw_linie( i );
		for(uint8_t j = 0; j < 132; j++ )
		{
			lcd_uc1701_wyslij_bajt(lcd_uc1701_dane,0);
		}
	}
}

// ustawia kursor na wybrana linie
void lcd_uc1701_ustaw_linie( uint8_t linia )
{
	// ustawia kursor na pozycje "0, linia"
    lcd_uc1701_wyslij_bajt(lcd_uc1701_komenda, 0xb0 + linia);

    lcd_uc1701_wyslij_bajt(lcd_uc1701_komenda, 0x10);

    lcd_uc1701_wyslij_bajt(lcd_uc1701_komenda, 0x04);
}
