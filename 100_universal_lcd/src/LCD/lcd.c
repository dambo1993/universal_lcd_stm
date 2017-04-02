/*
 * lcd.c
 *
 *  Created on: 12.01.2017
 *      Author: Przemek
 */

#include "lcd.h"
#include <stdlib.h>
#include "stm32f0xx.h"
#include "string.h"

// includowanie odpowiednich sprzetowych bibliotek

#if LCD_TYPE == LCD_TYPE_ST7920

#include "ST7920/ST7920.h"

#elif LCD_TYPE == LCD_TYPE_OLED
#include "OLED/oled.h"

#elif LCD_TYPE == LCD_TYPE_UC1701
#include "UC1701/lcd_uc1701.h"

#endif

/**
 * Wskaznik na funkcje odswiezajaca ekran.
 */
lcdb_odswiez_ekran_callback *my_lcdb_odswiez_ekran;

/**
 * Wskaznik na funkcje odswiezajaca obszar ekranu.
 */
lcdb_odswiez_obszar_callback *my_lcdb_odswiez_obszar;

/**
 * Tablica z wykorzystywana czcionka.
 */
const uint8_t ASCII[][5] = {
#include "LCD/font.h"
};

/**
 * Bufor wyswietlacza.
 */
uint8_t lcd_buff[BUFF_SIZE];


/**
 * Inicjalizacja wyswietlacza.
 */
void lcdb_init( void )
{

#if LCD_TYPE == LCD_TYPE_ST7920
	st_init();
	graphic_mode();

	my_lcdb_odswiez_ekran = ST7920_odswiez_ekran;
#elif LCD_TYPE == LCD_TYPE_OLED
	oled_init();
	my_lcdb_odswiez_ekran = oled_odswiez;
#elif LCD_TYPE == LCD_TYPE_UC1701
	lcd_uc1701_init();
	my_lcdb_odswiez_ekran = lcd_uc1701_buf_wyswietl;
#endif

}

/**
 * Czysci bufor w pamieci.
 */
void lcdb_czysc_bufor( void )
{
	uint16_t licznik = 0;
	for(licznik = 0;licznik < BUFF_SIZE;licznik++)
	{
		lcd_buff[licznik] = 0;
	}
}

/**
 * Czysci bufor w pamieci.
 */
void lcdb_zapal_pixel(uint16_t X, uint16_t Y,uint8_t zapal)
{
	if(zapal)
		lcd_buff[X + (128 *(Y / 8))] |= (1 << (Y%8));
	else
		lcd_buff[X + (128 *(Y / 8))] &= ~(1 << (Y%8));
}


/**
 * rysuje obrazek z tablicy do bufora
 * program microlcd ze strony http://hobby.abxyz.bplaced.net/index.php?pid=5&aid=12
 * ustawienia zapisu : horizontal
 * natomiast jesli chcemy skorzystac z pixel factory - ustawiamy vertical
 * dla wygody w tablicy dwa pierwsze bajty oznaczaja dlugosc i wysokosc w pikselach - trzeba to zawsze samemu dopisac, ale latwiej sie uzywa w programie
 * @param s - tablica z obrazkiem, ktora zawiera takze jego rozmiar.
 * @param x - wspolrzedna x.
 * @param y - wspolrzedna y.
 */
void lcdb_rysuj_obrazek_zmazywanie(const unsigned char s[],char  x, char y)
{
   uint16_t i,j,k;
   uint8_t px = 0;

   // odczytane zmienne odnosnie rozmiarow obrazka
   char w = s[0];
   char h = s[1];

   // przesuniecie wskaznika - pominiecie wymiarow
   s = s + 2;

   // przejazd po wierszach
   for(i=0,k=0; i < (h/8); i++)
   {
	   // przejazd po kolumnach
	   for(j = 0; j < w; j++)
	   {
		   // przejazd po pikselach
		   for( px = 0; px < 8; px++)
		   {
			   //jesli piksel jest zapalony
			   if( s[k] & ( 1 << px ) )
			   {
				   lcdb_zapal_pixel( x + j, y + ( 8 * i ) + px, 1 );
			   }
			   else
			   {
				   lcdb_zapal_pixel( x + j, y + ( 8 * i ) + px, 0 );
			   }
		   }
		   k++;
   	   }
   }
}

/**
 * Odswieza caly ekran.
 */
void lcdb_odswiez_ekran()
{
	my_lcdb_odswiez_ekran();
}

/**
 * Odswieza tylko wybrany obszar ekranu.
 * @param x1 - wspolrzedna x pierwszego wierzcholka.
 * @param y1 - wspolrzedna y pierwszego wierzcholka.
 * @param x2 - wspolrzedna x drugiego wierzcholka.
 * @param y2 - wspolrzedna x drugiego wierzcholka.
 */
void lcdb_odswiez_obszar(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2)
{
	my_lcdb_odswiez_obszar(x1,y1,x2,y2);
}

/**
 * Funkcja piszaca znak w zadanej pozycji
 * @param x - wspolrzedna x.
 * @param y - wspolrzedna ya.
 * @param c - znak do napisania.
 * @param kolor - czy tekst ma byc czarny(1), czy bialy(0).
 */
void lcdb_narysuj_znak(uint16_t x, uint16_t y,char c, uint8_t kolor)
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
				lcdb_zapal_pixel(x+i,y+j,kolor);
			}
			else
			{
				if( kolor )
				{
					lcdb_zapal_pixel(x+i,y+j,0 );
				}
				else
				{
					lcdb_zapal_pixel(x+i,y+j,1 );
				}
			}
			line >>= 1;
		}
	}
}

/**
 * Funkcja piszaca tekst w zadanej pozycji
 * @param x - wspolrzedna x.
 * @param y - wspolrzedna ya.
 * @param wsk - wskaznik na tekst do wyswietlenia.
 * @param kolor - czy tekst ma byc czarny(1), czy bialy(0).
 */
void lcdb_pisz_tekst(uint16_t x, uint16_t y, char* wsk, uint8_t kolor)
{
	char znak;
	uint16_t licznik = 0;
	while( (znak = *(wsk)) )
	{
		lcdb_narysuj_znak(x + ( licznik * 6 ),y,znak,kolor);
		licznik++;
		wsk++;
	}
}

/**
 * Funkcja piszaca liczbe w zadanej pozycji
 * @param x - wspolrzedna x.
 * @param y - wspolrzedna ya.
 * @param liczba - wartosc do wyswietlenia.
 * @param kolor - czy tekst ma byc czarny(1), czy bialy(0).
 */
void lcdb_pisz_liczbe(uint16_t x, uint16_t y, int16_t liczba, uint8_t kolor)
{
	char buff[10];

	itoa(liczba, buff, 10);

	lcdb_pisz_tekst(x,y,buff,kolor);
}



/**
 * Funkcja piszaca liczbe w zadanej pozycji, ale tylko okreslona ilosc - uzupelnia wybranym znakiem
 * w praktyce dziala jak wyrownanie do prawej
 * @param x - wspolrzedna x.
 * @param y - wspolrzedna ya.
 * @param liczba - wartosc do wyswietlenia.
 * @param kolor - czy tekst ma byc czarny(1), czy bialy(0).
 * @param znaki - ilosc znakow.
 * @param wypelniacz - jakim znakiem maja byc wypelnione puste pola (np spacja lub 0).
 */
void lcdb_pisz_liczbe_x_znakow(uint16_t x, uint16_t y, int16_t liczba, uint8_t kolor, uint8_t znaki, uint8_t wypelniacz)
{
	char buff[10];

	itoa(liczba, buff + 4, 10);

	// sprawdzamy rozmiar tekstu
	uint8_t rozmiar = strlen(buff + 4);

	uint8_t pozycja = 4;
	while( rozmiar < znaki )
	{
		buff[ pozycja - 1 ] = wypelniacz;
		pozycja--;
		rozmiar++;
	}

	lcdb_pisz_tekst(x,y,buff + pozycja,kolor);
}

/**
 * Funkcja piszaca znak w zadanej pozycji o wielkosci 16px.
 * @param x - wspolrzedna x.
 * @param y - wspolrzedna ya.
 * @param c - znak do napisania.
 * @param kolor - czy tekst ma byc czarny(1), czy bialy(0).
 */
void lcdb_narysuj_znak_16(uint16_t x, uint16_t y,char c, uint8_t kolor)
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
				lcdb_zapal_pixel(x+(2*i),y+(2*j),kolor);
				lcdb_zapal_pixel(x+(2*i)+1,y+(2*j),kolor);
				lcdb_zapal_pixel(x+(2*i),y+(2*j)+1,kolor);
				lcdb_zapal_pixel(x+(2*i)+1,y+(2*j)+1,kolor);
			}
			else
			{
				if( kolor )
				{
					lcdb_zapal_pixel(x+(2*i),y+(2*j),0);
					lcdb_zapal_pixel(x+(2*i)+1,y+(2*j),0);
					lcdb_zapal_pixel(x+(2*i),y+(2*j)+1,0);
					lcdb_zapal_pixel(x+(2*i)+1,y+(2*j)+1,0);
				}
				else
				{
					lcdb_zapal_pixel(x+(2*i),y+(2*j),1);
					lcdb_zapal_pixel(x+(2*i)+1,y+(2*j),1);
					lcdb_zapal_pixel(x+(2*i),y+(2*j)+1,1);
					lcdb_zapal_pixel(x+(2*i)+1,y+(2*j)+1,1);
				}
			}
			line >>= 1;
		}
	}
}

/**
 * Funkcja piszaca tekst w zadanej pozycji o wielkosci 16px.
 * @param x - wspolrzedna x.
 * @param y - wspolrzedna ya.
 * @param wsk - wskaznik na tekst do wyswietlenia.
 * @param kolor - czy tekst ma byc czarny(1), czy bialy(0).
 */
void lcdb_pisz_tekst_16(uint16_t x, uint16_t y, char* wsk, uint8_t kolor)
{
	char znak;
	uint16_t licznik = 0;
	while( (znak = *(wsk)) )
	{
		lcdb_narysuj_znak_16(x + ( licznik * 12 ),y,znak,kolor);
		licznik++;
		wsk++;
	}
}

/**
 * Funkcja piszaca liczbe w zadanej pozycji o wielkosci 16px.
 * @param x - wspolrzedna x.
 * @param y - wspolrzedna ya.
 * @param liczba - wartosc do wyswietlenia.
 * @param kolor - czy tekst ma byc czarny(1), czy bialy(0).
 */
void lcdb_pisz_liczbe_16(uint16_t x, uint16_t y, int16_t liczba, uint8_t kolor)
{
	char buff[10];

	itoa(liczba, buff, 10);

	lcdb_pisz_tekst_16(x,y,buff,kolor);
}

/**
 * Funkcja piszaca liczbe w zadanej pozycji, ale tylko okreslona ilosc - uzupelnia wybranym znakiem
 * w praktyce dziala jak wyrownanie do prawej
 * @param x - wspolrzedna x.
 * @param y - wspolrzedna ya.
 * @param liczba - wartosc do wyswietlenia.
 * @param kolor - czy tekst ma byc czarny(1), czy bialy(0).
 * @param znaki - ilosc znakow.
 * @param wypelniacz - jakim znakiem maja byc wypelnione puste pola (np spacja lub 0).
 */
void lcdb_pisz_liczbe_x_znakow_16(uint16_t x, uint16_t y, int16_t liczba, uint8_t kolor, uint8_t znaki, uint8_t wypelniacz)
{
	char buff[10];

	itoa(liczba, buff + 4, 10);

	// sprawdzamy rozmiar tekstu
	uint8_t rozmiar = strlen(buff + 4);

	uint8_t pozycja = 4;
	while( rozmiar < znaki )
	{
		buff[ pozycja - 1 ] = wypelniacz;
		pozycja--;
		rozmiar++;
	}

	lcdb_pisz_tekst_16(x,y,buff + pozycja,kolor);
}



// rysuje linie o poczatku w punkcie x1,y1, a koncu w x2,y2, o kolorze 0 lub 1
void lcdb_rysuj_linie(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2,uint8_t zapal)
{
    // zmienne pomocnicze
    int16_t d, dx, dy, ai, bi, xi, yi;
    int16_t x = x1, y = y1;
    // ustalenie kierunku rysowania
    if (x1 < x2)
    {
        xi = 1;
        dx = x2 - x1;
    }
    else
    {
        xi = -1;
        dx = x1 - x2;
    }
    // ustalenie kierunku rysowania
    if (y1 < y2)
    {
        yi = 1;
        dy = y2 - y1;
    }
    else
    {
        yi = -1;
        dy = y1 - y2;
    }
    // pierwszy piksel
    lcdb_zapal_pixel(x,y,zapal);
    // oœ wiod¹ca OX
    if (dx > dy)
    {
        ai = (dy - dx) * 2;
        bi = dy * 2;
        d = bi - dx;
        // pêtla po kolejnych x
        while (x != x2)
        {
            // test wspó³czynnika
            if (d >= 0)
            {
                x += xi;
                y += yi;
                d += ai;
            }
            else
            {
                d += bi;
                x += xi;
            }
            lcdb_zapal_pixel(x,y,zapal);
        }
    }
    // oœ wiod¹ca OY
    else
    {
        ai = ( dx - dy ) * 2;
        bi = dx * 2;
        d = bi - dy;
        // pêtla po kolejnych y
        while (y != y2)
        {
            // test wspó³czynnika
            if (d >= 0)
            {
                x += xi;
                y += yi;
                d += ai;
            }
            else
            {
                d += bi;
                y += yi;
            }
            lcdb_zapal_pixel(x,y,zapal);
        }
    }
}

//kierunek = 1 dla pionowych, 0 dla poziomych
void lcdb_rysuj_linie_3p(uint16_t x, uint16_t y , uint16_t dlugosc, uint8_t kierunek, uint8_t zapal)
{
	// TODO do poprawienia - to mozna optymalniej, a nie tym algorytmem
	if(kierunek)
		lcdb_rysuj_linie(x, y, x, y+dlugosc-1,zapal);
	else
		lcdb_rysuj_linie(x, y, x+dlugosc-1, y,zapal);
}

// rysuje okrag o podanym srodku i podanym promieniu
void lcdb_rysuj_okrag(uint16_t x0, uint16_t y0, uint16_t r,uint8_t zapal)
{
	  int16_t f = 1 - r;
	  int16_t ddF_x = 1;
	  int16_t ddF_y = -2 * r;
	  int16_t x = 0;
	  int16_t y = r;

	  lcdb_zapal_pixel(x0 , y0+r, zapal);
	  lcdb_zapal_pixel(x0 , y0-r, zapal);
	  lcdb_zapal_pixel(x0+r, y0 , zapal);
	  lcdb_zapal_pixel(x0-r, y0 , zapal);

	  while (x<y) {
	    if (f >= 0) {
	      y--;
	      ddF_y += 2;
	      f += ddF_y;
	    }
	    x++;
	    ddF_x += 2;
	    f += ddF_x;

	    lcdb_zapal_pixel(x0 + x, y0 + y, zapal);
	    lcdb_zapal_pixel(x0 - x, y0 + y, zapal);
	    lcdb_zapal_pixel(x0 + x, y0 - y, zapal);
	    lcdb_zapal_pixel(x0 - x, y0 - y, zapal);
	    lcdb_zapal_pixel(x0 + y, y0 + x, zapal);
	    lcdb_zapal_pixel(x0 - y, y0 + x, zapal);
	    lcdb_zapal_pixel(x0 + y, y0 - x, zapal);
	    lcdb_zapal_pixel(x0 - y, y0 - x, zapal);
	  }
}

// rysuje prostokat o poczatku w podanym punkcie i o podanych wymiarach
void lcdb_rysuj_prostokat(uint16_t x, uint16_t y, uint16_t dlugosc, uint16_t wysokosc,uint8_t zapal)
{
	lcdb_rysuj_linie(x, y, x + dlugosc, y, zapal);//gora
	lcdb_rysuj_linie(x, y + wysokosc, x + dlugosc, y + wysokosc, zapal);//dol
	lcdb_rysuj_linie(x, y, x, y + wysokosc, zapal);//lewo
	lcdb_rysuj_linie(x+dlugosc, y, x + dlugosc , y + wysokosc, zapal);
}
