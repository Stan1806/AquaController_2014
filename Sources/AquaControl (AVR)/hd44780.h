/*	АКВАКОНТРОЛЛЕР 2014 В. 1.0.0.

	Библиотека для работы с ЖКИ на контроллере HD44780.
	Используется 4-х битная шина передачи данных.
	Индикатор WH2004L-TMI-CTW.
	
	Пустоводов С.А. 2014г.
*/

#ifndef hd44780_H
#define hd44780_H
#endif
// стандартные библиотеки
#include <avr/io.h>
#include "Functions.h"
#include <util/delay.h>

// определители
#define LCD_RS() LCD_SYGNAL_PORT|= RS // установить сигнал данные/команда
#define LCD_NRS() LCD_SYGNAL_PORT&=~RS // сбросить сигнал данные/команда
#define LCD_E() LCD_SYGNAL_PORT|= E // установить сигнал строба
#define LCD_NE() LCD_SYGNAL_PORT&=~E // сбросить сигнал строба
#define default1 0x38 // часто используемые настройки (шина данных 8-бит, 4 ряда, шрифт5х7)
#define default2 0x0C // включить дисплей, погасить курсор
#define delay 200 // значение задержки для паузы между командами
#define char_num 20 // кол-во символов в строке

// глобальные переменные
char string_1[char_num]="                    "; // строка 1
char string_2[char_num]="<T1> 00.0  PH       "; // строка 2
char string_3[char_num]="<T2> 00.0  Rdx      "; // строка 3
char string_4[char_num]="<T3> 00.0  ---------"; // строка 4

// прототипы функций
void write(void); // функция записи данных ЖКИ
void init_LCD(void); // инициализация ЖКИ
void data_to_LCD(unsigned char data_in); // отправить данные ЖКИ
void comand_to_LCD(unsigned char data_in); // отправкить команду ЖКИ
void string_to_LCD(unsigned char line, unsigned char pos, char str[]); // вывести строку ЖКИ

void init_LCD() // функция инициализации ЖКИ
{
	// конфигурация выводовд
	LCD_SYGNAL_DDR |= RS; 
	LCD_SYGNAL_DDR |= E; 
	
	// инициализация ЖКИ
	LCD_NRS();
	_delay_us(delay);
	LCD_DATA_PORT=0b00100000|(LCD_DATA_PIN&DATA_MASK); // установить значение
	write(); // запись
	comand_to_LCD(0b00101000); _delay_us(40);
	comand_to_LCD(0b00001100); _delay_us(40);
	comand_to_LCD(0b00000001); _delay_ms(2);
	comand_to_LCD(0b00000110); _delay_us(40); 	
	_delay_ms(40); // задержка
}

void write() // функция записи данных ЖКИ
{
	LCD_E(); // установка строба
	_delay_us(delay); 
	LCD_NE(); // сброс строба
	_delay_us(delay);
}

void comand_to_LCD(unsigned char data_in) // функция отправки команд ЖКИ
{
	LCD_NRS(); // сбросить сигнал данные/команда
	_delay_us(delay);
	// отправить старшую тетраду
	LCD_DATA_PORT=(data_in&0xF0)|(LCD_DATA_PIN&DATA_MASK);
	write(); 
	// отправить младшую тетраду
	LCD_DATA_PORT=(data_in<<4)|(LCD_DATA_PIN&DATA_MASK);
	write(); 
}

void data_to_LCD(unsigned char data_in) // функция отправки данных ЖКИ
{
	LCD_RS(); // установить сигнал данные/команда
	_delay_us(delay);
	// отправить старшую тетраду
	LCD_DATA_PORT=(data_in&0xF0)|(LCD_DATA_PIN&DATA_MASK);
	write();
	// отправить младшую тетраду
	LCD_DATA_PORT=(data_in<<4)|(LCD_DATA_PIN&DATA_MASK);
	write(); 
}

void string_to_LCD(unsigned char line, unsigned char pos, char str[]) // функция вывода строк ЖКИ
{
	switch(line) // выбор отображаемой строки
	{
		case 1: pos=pos+0x80; break;
		case 2: pos=pos+0xC0; break;
		case 3: pos=pos+0x94; break;
		case 4: pos=pos+0xD4; break;
		default:; break;
	}
	comand_to_LCD(pos); // установка позиции курсора

	for(unsigned char t=0;t<char_num;t++) data_to_LCD(str[t]); // вывод данных
}