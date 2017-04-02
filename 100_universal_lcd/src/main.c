#include "stm32f0xx.h"

#include "delay_systick/delay_systick.h"
#include "RCC/RCC.h"

#include "LCD/lcd.h"

const uint8_t obrazek[] ={
#include "obr.h"
};

int main(void)
{
	// podstawowa inicjalizacja
	RCC_init();
	delay_init();

	// obsluga wyswietlacza
	lcdb_init();
	lcdb_czysc_bufor();
	lcdb_pisz_tekst(0,0,"test napisu na lcdku",1);
	lcdb_pisz_liczbe_x_znakow(10,10,15,1,4,' ');
	lcdb_odswiez_ekran();

	_delay_ms(1000);
	while (1)
	{

	}
}
