#ifndef F_CPU
#define F_CPU 8000000UL // 8 MHz clock speed
#endif

#define D4 eS_PORTC4
#define D5 eS_PORTC5
#define D6 eS_PORTC6
#define D7 eS_PORTC7

#define RS eS_PORTC0
#define EN eS_PORTC2

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <string.h>
#include <stdlib.h>
#include <avr/eeprom.h>
#include "lcd.h"
#include <stdio.h>

char first_number[10];
char second_number[10];
char operation;
char temp;
char result[10];

char keypad()
{
	unsigned char k_in = 0xFF;
	while(1)
	{
		//scan 1st row
		PORTB = 0xEF;	//select 1st row
		_delay_ms(10);
		k_in = PINB & 0x0F; //check button on what column of 1st row
		switch(k_in)
		{
			case 0x07:
			return ('1');
			case 0x0B:
			return ('2');
			case 0x0D:
			return ('3');
			case 0x0E:
			return ('+');
		}
		
		//scan 2nd row
		PORTB = 0xDF;	//select second row
		_delay_ms(10);
		k_in = PINB & 0x0F;	//check button on what column of 2nd row
		switch(k_in)
		{
			case 0x07:
			return ('4');
			case 0x0B:
			return ('5');
			case 0x0D:
			return ('6');
			case 0x0E:
			return ('-');
		}
		//scan 3rd row
		PORTB = 0xBF; //select 3rd row
		_delay_ms(10);
		k_in = PINB & 0x0F; //check button on what column of 3rd row
		switch(k_in)
		{
			case 0x07:
			return ('7');
			case 0x0B:
			return ('8');
			case 0x0D:
			return ('9');
			case 0x0E:
			return ('*');
		}
		//scan 4th row
		PORTB = 0x7F;	//select 4th row
		_delay_ms(10);
		k_in = PINB & 0x0F;	//check button on what column of 4th row
		switch(k_in)
		{
			case 0x07:
			return ('.');
			case 0x0B:
			return ('0');
			case 0x0D:
			return ('=');
			case 0x0E:
			return ('/');
		}
		
	}
}


void enter()
{
	Lcd4_Set_Cursor(1,1);
	for(int i = 0; i < 9; i++)
	{	
		temp = keypad();
		_delay_ms(300);
		if ( ( temp != '+') && ( temp != '-') &&  ( temp != '*') && ( temp != '/') )
		{
			first_number[i] = temp;
			Lcd4_Write_Char(temp);
		}
		else
		{
			operation = temp;
			Lcd4_Write_Char(temp);
			break;
		}
	}
	for (int j = 0; j < 9; j ++)
	{
		temp = keypad();
		_delay_ms(300);
		if ( temp != '=')
		{
			second_number[j] = temp;
				Lcd4_Write_Char(temp);
		}
		else
		{
			Lcd4_Set_Cursor(2,1);
			Lcd4_Write_Char(temp);
			break;
		}
	}

}



void calculate()
{
	int x = atoi(first_number);
	int y = atoi(second_number);
	float a = atof(first_number);
	float b = atof(second_number);
	switch(operation)
	{
		case '+':
		{
			int integer_result = x + y;
			float float_result = a + b; 
			int remain= (float_result - integer_result) * 1000;
				if ( remain >= 1000)
				{
					integer_result = integer_result + (remain)/1000;
					remain = remain % 1000;
				}
			sprintf(result, "%d.%d", integer_result, remain);
			break;
		}
		case '-':
		{
			int integer_result = x - y;
			float float_result = a - b;
			int remain= ((float_result - integer_result) * 1000);
			if (remain < 0)
			{
				integer_result = integer_result - 1;
				remain = (float_result - integer_result) * 1000;
			}
			sprintf(result, "%d.%d", integer_result, remain);
			break;
		}
		case '*':
		{
			int integer_result = x * y;
			float float_result = a * b;
			int remain= (float_result - integer_result) * 1000;
			if (remain >= 1000)
			{
				int bonus = remain / 1000;
				integer_result = integer_result + bonus;
				remain = remain % 1000;
			}
			sprintf(result, "%d.%d", integer_result, remain);
			break;
		}
		case '/':
		{
			int integer_result = x / y;
			int remain = ((x % y)*1000)/y;
			sprintf(result, "%d.%d", integer_result, remain);
			break;
		}
	}
}

int main()
{
	SP = RAMEND;
	// config DDRC for LCD
	DDRC= 0xFF;
	PINC = 0xFF;
	Lcd4_Init();
	//config DDRB for keypad
	DDRB = 0xF0;
	PORTB = 0xFF;
	//array counter
	MCUCR = 0b00000010; //make INT0 falling edge triggered
	GICR = (1<<INT0);
	sei();
	
	while (1)
	{
		enter();	
		Lcd4_Set_Cursor(2,3);
		calculate();
		Lcd4_Write_String(result);
	}
	
}
ISR(INT0_vect)
{
	Lcd4_Clear();
	memset(first_number, 0, sizeof(first_number));
	memset(second_number, 0, sizeof(second_number));
	memset(result, 0, sizeof(result));
	temp = 0;
	operation = NULL;
}