/*
 * \file
 * \brief Uniwersalna biblioteka do wyswietlaczy monochromatycznych.
 *
 *
 */

// jest to uniwersalna biblioteka z warstwa "graficzna" dla roznych modeli lcd, trzeba pod nia podpiac tylko funkcje sprzetowe
// oczywiscie, zeby w pelni wykorzystac lcd trzeba tez korzystac z warstwy sprzetowej
// zobaczymy jak to sie rozrosnie

#ifndef LCD_H_
#define LCD_H_

#include "stm32f0xx.h"

// callbacki do funkcji sprzetowych wyswietlaczy

// utworzenie typu CALLbacka, dla funkcji inicjalizacyjnych
typedef void (lcdb_init_callback) (void);

/**
 * Utworzenie typu CALLbacka, dla funkcji odswiezajacej ekran
 */
typedef void (lcdb_odswiez_ekran_callback) (void);

// utworzenie typu CALLbacka, dla funkcji odswiezajacej obszar
typedef void (lcdb_odswiez_obszar_callback) (uint8_t , uint8_t , uint8_t , uint8_t );

// tutaj definiujemy jaki jest nasz wyswietlacz - w zaleznosci od niego
// biblioteka bedzie odpowiednio konfigurowana

#define LCD_TYPE_ST7920 	0
#define LCD_TYPE_OLED		1
#define LCD_TYPE_UC1701 	2

// wybranie naszego wyswietlacza
#define LCD_TYPE LCD_TYPE_UC1701


#if LCD_TYPE == LCD_TYPE_ST7920

#define LCD_MAX_WIDTH 128
#define LCD_MAX_HEIGHT 64
#define BUFF_SIZE 1024

#elif LCD_TYPE == LCD_TYPE_OLED

#define LCD_MAX_WIDTH 128
#define LCD_MAX_HEIGHT 64
#define BUFF_SIZE 1024

#elif LCD_TYPE == LCD_TYPE_UC1701

#define LCD_MAX_WIDTH 128
#define LCD_MAX_HEIGHT 64
#define BUFF_SIZE 1024

#endif

//-------------------------------------------------------------------------------------------------
// Sekcja funkcji sprzetowych dla wyswietlaczy
//-------------------------------------------------------------------------------------------------

// funkcja inicjalizujaca wyswietlacz
void lcdb_init( void );

//-------------------------------------------------------------------------------------------------
// Sekcja funkcji softwarowych operujacych na buforze w pamieci
//-------------------------------------------------------------------------------------------------

// ogolne
void lcdb_czysc_bufor(void);

// tekst - normalna wielkosc
void lcdb_narysuj_znak(uint16_t x, uint16_t y,char c, uint8_t kolor);
void lcdb_pisz_tekst(uint16_t x, uint16_t y, char* wsk, uint8_t kolor);
void lcdb_pisz_liczbe(uint16_t x, uint16_t y, int16_t liczba, uint8_t kolor);
void lcdb_pisz_liczbe_x_znakow(uint16_t x, uint16_t y, int16_t liczba, uint8_t kolor, uint8_t znaki, uint8_t wypelniacz);


// tekst - podwojna wielkosc - 16 pikseli
void lcdb_narysuj_znak_16(uint16_t x, uint16_t y,char c, uint8_t kolor);
void lcdb_pisz_tekst_16(uint16_t x, uint16_t y, char* wsk, uint8_t kolor);
void lcdb_pisz_liczbe_16(uint16_t x, uint16_t y, int16_t liczba, uint8_t kolor);
void lcdb_pisz_liczbe_16_x_znakow(uint16_t x, uint16_t y, int16_t liczba, uint8_t kolor, uint8_t znaki);

// bitmapy
void lcdb_zapal_pixel(uint16_t X, uint16_t Y,uint8_t zapal);
void lcdb_rysuj_obrazek_zmazywanie(const unsigned char s[],char  x, char y);

// grafika
void lcdb_rysuj_linie(uint16_t, uint16_t, uint16_t, uint16_t,uint8_t);
void lcdb_rysuj_linie_3p(uint16_t, uint16_t, uint16_t,uint8_t,uint8_t);
void lcdb_rysuj_okrag(uint16_t , uint16_t , uint16_t,uint8_t);
void lcdb_rysuj_prostokat(uint16_t , uint16_t, uint16_t, uint16_t,uint8_t);


//-------------------------------------------------------------------------------------------------
// Sekcja funkcji sprzetowych,ktore korzystaja z bufora pamieci
//-------------------------------------------------------------------------------------------------

void lcdb_odswiez_ekran(void);

void lcdb_odswiez_obszar(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2);


#endif /* LCD_H_ */
