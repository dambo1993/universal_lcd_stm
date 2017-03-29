/*
 * oled.c
 *
 *  Created on: 14 styczen
 *      Author: Przemek
 */

#include "oled.h"

#include "stm32f0xx.h"

#include <stdio.h>
#include <stdlib.h>
#include "delay_systick/delay_systick.h"
#include "../lcd.h"


extern uint8_t lcd_buff[1024];
extern const uint8_t ASCII[][5];


#if DMA_ON == 1
volatile uint8_t DMA_flaga = 0;
#endif

void oled_spi_wyslij(uint8_t data)
{
	while( SPI1->SR & SPI_SR_BSY );
	*(uint8_t *)&SPI1->DR = (uint8_t)data;
	uint8_t x = 10;
	while(x--);
}


void oled_wyslij_bajt(uint8_t c_d, uint8_t data)
{
#if CE_USE == 1
    LCD_CE_CLR;
#endif

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
	uint8_t x = 50;
	while(x--);

#if CE_USE == 1
    LCD_CE_SET;
#endif

}

void oled_display()
{
	oled_wyslij_bajt(0,SSD1306_SETLOWCOLUMN | 0x0);
	oled_wyslij_bajt(0,SSD1306_SETHIGHCOLUMN| 0x0);
	oled_wyslij_bajt(0,SSD1306_SETSTARTLINE | 0x0);

#if DMA_ON == 0

#if CE_USE == 1
    LCD_CE_CLR;
#endif
	LCD_DC_SET;

	for(uint16_t i = 0; i < 1024; i++)
	{
		oled_spi_wyslij(lcd_buff[i]);
	}
	LCD_DC_CLR;
#if CE_USE == 1
    LCD_CE_SET;
#endif

#else
	// oczekiwanie az zwolni sie DMA
	while( DMA_flaga );
	LCD_CE_CLR;
	LCD_DC_SET;

	// adres pamieci
	DMA1_Channel3->CMAR = (uint32_t) ( lcd_buff );

	// wlaczenie inkrementacji wskaznikow dla pamieci
	DMA1_Channel3->CCR |= DMA_CCR_MINC;

	// ustawienie licznika
	DMA1_Channel3->CNDTR = 1024;

	// uruchomienie
	DMA1_Channel3->CCR |= DMA_CCR_EN;

	DMA_flaga = 1;
#endif
}


#if DMA_ON == 1
void oled_DMA_init( void )
{
	RCC->AHBENR |= RCC_AHBENR_DMA1EN;
	// adres urzadzenia
	DMA1_Channel3->CPAR = (uint32_t ) &SPI1->DR;

	// adres pamieci
	DMA1_Channel3->CMAR = (uint32_t) ( lcd_bufor );

	// licznik "przesylow"
	DMA1_Channel3->CNDTR = 1024;

	// rozmiar_danych wejsciowych
//	DMA1_Channel1->CCR |= DMA_CCR_MSIZE_1; // 8 bitow domyslnie (00)

	// rozmiar_danych wyjsciowych
//	DMA1_Channel1->CCR |= DMA_CCR_PSIZE_1; // 8 bitow domyslnie (00)

	// inkrementacje wskaznikow dla pamieci
	DMA1_Channel3->CCR |= DMA_CCR_MINC;

	// z pamieci na urzadzenie
	DMA1_Channel3->CCR |= DMA_CCR_DIR;

	// wlaczenie przerwania po zakonczeniu przesylania
	DMA1_Channel3->CCR |= DMA_CCR_TCIE;

	// wlaczenie kanalu
//	DMA1_Channel3->CCR |= DMA_CCR_EN;
	_delay_ms(100);
	_delay_ms(100);

	NVIC_EnableIRQ( DMA1_Channel2_3_IRQn );
}

// obsluga przerwania
void DMA1_Channel2_3_IRQHandler( void )
{
	// zerowanie flagi zajatosci DMA
	DMA_flaga = 0;

	// wylaczenie DMA i flagi przerwania
	DMA1_Channel3->CCR &= ~DMA_CCR_EN;
	DMA1->IFCR |= DMA_IFCR_CTCIF3;

	// adres pamieci
	DMA1_Channel3->CMAR = (uint32_t)( lcd_bufor );

	// inkrementacje wskaznikow dla pamieci
	DMA1_Channel3->CCR |= DMA_CCR_MINC;

	LCD_CE_SET;
}

#endif

void oled_invert(uint8_t kolor)
{
	  if (kolor)
	  {
		  oled_wyslij_bajt(0,SSD1306_INVERTDISPLAY);
	  }
	  else
	  {
		  oled_wyslij_bajt(0,SSD1306_NORMALDISPLAY);
	  }
}

void oled_ustaw_kontrast(uint8_t kontrast)
{
	oled_wyslij_bajt(0,SSD1306_SETCONTRAST);
	oled_wyslij_bajt(0,kontrast);
}

void oled_set_pixel(int8_t x, int8_t y, uint8_t pixel)
{
	if(pixel)
	{
		lcd_buff[ x + (y/8) * SSD1306_WITCH] |= ( 1 << (y%8));
	}
	else
	{
		lcd_buff[ x + (y/8) * SSD1306_WITCH] &= ~( 1 << (y%8));
	}
}

void oled_odswiez_obszar(uint8_t strona_numer, uint8_t strona_ilosc, uint8_t kolumna_startowa, uint8_t kolumna_koncowa)
{
	uint8_t strona_licznik, kolumna_licznik;
	uint8_t *ram_buf_start;
	for(strona_licznik = strona_numer; strona_licznik < ( strona_numer+strona_ilosc); strona_licznik++ )
	{
		oled_wyslij_bajt(0, SSD1306_SETLOWCOLUMN | (kolumna_startowa & 0x0f));
		oled_wyslij_bajt(0, SSD1306_SETHIGHCOLUMN | (kolumna_startowa >> 4));
		oled_wyslij_bajt(0, 0xB0 + strona_ilosc);
		LCD_DC_SET;
		ram_buf_start = &lcd_buff[ (strona_licznik * 128) + kolumna_startowa];
		for(kolumna_licznik = kolumna_startowa; kolumna_licznik < ( kolumna_koncowa); kolumna_licznik++)
		{
			oled_spi_wyslij(*ram_buf_start++);
		}
	}
}



void oled_narysuj_znak(uint8_t x, uint8_t y,char c, uint8_t kolor, uint8_t tlo, uint8_t rozmiar)
{
	uint8_t line;
	for(uint8_t i = 0; i < 6; i++)
	{

		if(i == 5)
		{
			line = 0x0;
		}
		else
		{
			line = ASCII[c - 0x20][i];
		}
		for(int8_t j = 0; j < 8; j++)
		{
			if(line & 0x01)
			{
				if(rozmiar == 1)
				{
					oled_set_pixel(x+i,y+j,kolor);
				}
				else
				{
//					oled_rysuj_prostokat( x + (i * rozmiar), y+(j * rozmiar),rozmiar,rozmiar,kolor);
				}
			}
			else if(tlo != kolor)
			{
				if(rozmiar == 1)
				{
					oled_set_pixel(x + i, y + j, tlo);
				}
				else
				{
//					oled_rysuj_prostokat( x + (i * rozmiar), y+(j * rozmiar),rozmiar,rozmiar,tlo);
				}
			}
			line >>= 1;
		}

	}
}

// kierunek = 0 dla przewijania w prawo, 1 dla w lewo
void oled_scroll_boki_ustawienia(uint8_t kierunek, uint8_t start, uint8_t predkosc, uint8_t koniec)
{
	oled_wyslij_bajt(0,0x26 | kierunek);
	oled_wyslij_bajt(0,0x00);  // dummby bajt
	oled_wyslij_bajt(0,start); //start
	oled_wyslij_bajt(0,predkosc); //predkosc
	// 0 - 5kl, 1 - 64kl, 2-128kl, 3 - 256kl, 4 - 3kl, 5 - 4kl, 6 - 25kl, 7 - 2 kl
	oled_wyslij_bajt(0,koniec);
	oled_wyslij_bajt(0,0x00);  // dummby bajt
	oled_wyslij_bajt(0,0xff);  // dummby bajt

}

void oled_scroll_pion_ustawienia(uint8_t kierunek, uint8_t start, uint8_t predkosc, uint8_t koniec, uint8_t vertical_ofset)
{
	if(kierunek)
	{
		oled_wyslij_bajt(0,0x2a);
	}
	else
	{
		oled_wyslij_bajt(0,0x29);
	}
	oled_wyslij_bajt(0,0x00);  // dummby bajt
	oled_wyslij_bajt(0,start); //start
	oled_wyslij_bajt(0,predkosc); //predkosc
	// 0 - 5kl, 1 - 64kl, 2-128kl, 3 - 256kl, 4 - 3kl, 5 - 4kl, 6 - 25kl, 7 - 2 kl
	oled_wyslij_bajt(0,koniec);
	oled_wyslij_bajt(0,vertical_ofset);  // dummby bajt
//	oled_wyslij_bajt(0,0xff);  // dummby bajt

}

void oled_scroll_wl_wyl(uint8_t stan)
{
	if(stan)
	{
		oled_wyslij_bajt(0,0x2f);
	}
	else
	{
		oled_wyslij_bajt(0,0x2e);
	}
}

void oled_init(void)
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

	// pin dc
	LCD_DC_OUT;

	//przypisanie funkcji alternatywnych dla pinow
	// (niepotrzebne, bo wszedzie sa alternate function 0)

	// ustawienie SPI
	// wlaczenie zegara
	RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;

	// ustawienie predkosci taktowania zegara (domyslnie FPU/2)
//	SPI1->CR1 |= SPI_CR1_BR_0 | SPI_CR1_BR_1; // przez 16
//	SPI1->CR1 |= SPI_CR1_BR_0; // przez 4
	// domyslnie jest przez 2

	// softwarowy pin NSS
	SPI1->CR1 |= SPI_CR1_SSM;
	SPI1->CR1 |= SPI_CR1_SSI;

	// tryb master
	SPI1->CR1 |= SPI_CR1_MSTR;

	// tylko nadawanie
	SPI1->CR1 |= SPI_CR1_BIDIOE;

	// 8 bitow danych:
	SPI1->CR2 |= SPI_CR2_DS_2 | SPI_CR2_DS_1 | SPI_CR2_DS_0;

#if DMA_ON == 1
	// wlaczenie generowania zadania DMA
	SPI1->CR2 |= SPI_CR2_TXDMAEN;
#endif
	// uruchomienie SPI
	SPI1->CR1 |= SPI_CR1_SPE;

#if DMA_ON == 1
	oled_DMA_init();
#endif


	LCD_RST_OUT;
#if CE_USE == 1
	LCD_CE_OUT;
	LCD_CE_SET;
#endif
	LCD_DC_OUT;

	LCD_RST_SET;
	_delay_ms(50);
	LCD_RST_CLR;
	_delay_ms(50);
	LCD_RST_SET;
	_delay_ms(50);

    // Init sequence for 128x64 OLED module
	oled_wyslij_bajt(0,SSD1306_DISPLAYOFF);                    // 0xAE
	oled_wyslij_bajt(0,SSD1306_SETDISPLAYCLOCKDIV);            // 0xD5
	oled_wyslij_bajt(0,0x80);                                  // the suggested ratio 0x80
	oled_wyslij_bajt(0,SSD1306_SETMULTIPLEX);                  // 0xA8
	oled_wyslij_bajt(0,0x3F);
	oled_wyslij_bajt(0,SSD1306_SETDISPLAYOFFSET);              // 0xD3
	oled_wyslij_bajt(0,0x0);                                   // no offset
	oled_wyslij_bajt(0,SSD1306_SETSTARTLINE | 0x0);            // line #0
	oled_wyslij_bajt(0,SSD1306_CHARGEPUMP);                    // 0x8D

	oled_wyslij_bajt(0,0x14);

	oled_wyslij_bajt(0,SSD1306_MEMORYMODE);                    // 0x20
	oled_wyslij_bajt(0,0x00);                                  // 0x0 act like ks0108
	oled_wyslij_bajt(0,SSD1306_SEGREMAP | 0x1);
	oled_wyslij_bajt(0,SSD1306_COMSCANDEC);
	oled_wyslij_bajt(0,SSD1306_SETCOMPINS);                    // 0xDA
	oled_wyslij_bajt(0,0x12);
	oled_wyslij_bajt(0,SSD1306_SETCONTRAST);                   // 0x81

	oled_wyslij_bajt(0,0xCF);

	oled_wyslij_bajt(0,SSD1306_SETPRECHARGE);                  // 0xd9

	oled_wyslij_bajt(0,0xF1);

	oled_wyslij_bajt(0,SSD1306_SETVCOMDETECT);                 // 0xDB
	oled_wyslij_bajt(0,0x40);
	oled_wyslij_bajt(0,SSD1306_DISPLAYALLON_RESUME);           // 0xA4
	oled_wyslij_bajt(0,SSD1306_NORMALDISPLAY);                 // 0xA6

	oled_wyslij_bajt(0,SSD1306_DISPLAYON);//--turn on oled panel
}


void oled_set_pixel_fast(int16_t x, int16_t y, uint8_t pixel)
{
	if(pixel)
	{
		lcd_buff[ x + (y/8) * SSD1306_WITCH] |= ( 1 << (y%8));
	}
	else
	{
		lcd_buff[ x + (y/8) * SSD1306_WITCH] &= ~( 1 << (y%8));
	}
	oled_wyslij_bajt(0,SSD1306_SETLOWCOLUMN | 0x0);
	oled_wyslij_bajt(0,SSD1306_SETHIGHCOLUMN| 0x0);
	oled_wyslij_bajt(0,SSD1306_SETSTARTLINE | 0x0);
	LCD_DC_SET;
	for(uint16_t i = 0; i < 1024; i++)
	{
		oled_spi_wyslij(lcd_buff[i]);
	}
	LCD_DC_CLR;
}


