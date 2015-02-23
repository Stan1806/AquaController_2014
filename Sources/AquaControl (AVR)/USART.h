/*	АКВАКОНТРОЛЛЕР 2014 В. 1.0.0.

	Библиотека для работы с USART.
	Для передачи/приема данных используются прерывания по опустошению буффера,
	по завершению приема/передачи значения.
	
	Пустоводов С.А. 2014г.
*/

#ifndef USART_H
#define USART_H
#endif
// определители
#define USART_DDR DDRD // порт, располагающий модулем 
#define RX _BV(PD0) // вывод RX
#define TX _BV(PD1) // вывод TX
#define bauddivider (FREQ_CPU/(16*baudrate)-1) // расчет скорости передачи
#define HI(x) ((x)>>8) // запись в старший регистр
#define LO(x) ((x)& 0xFF) // запись в младший регистр
#define buffer_MAX 512 // длина текстового буффера
#define buffer_MAX_IN 512 // длина текстового буффера
// глобальные переменные
volatile unsigned char buffer[buffer_MAX]=""; // данные буффера
volatile unsigned int buffer_index=0; // индекс значения
volatile unsigned char usart_data_in[buffer_MAX_IN]=""; // принятые данные
volatile unsigned char TRANSMIT=0x00;
volatile unsigned int usart_count=0;
volatile unsigned int usart_data_in_num=0;
// прототипы функций
void buffer_out(void);
void buffer_start(void);

// прерывание по опустошению буффера
ISR (USART_UDRE_vect)		
{
	buffer_out();
}
// прерывание по завершению приема
ISR (USART_RXC_vect)		
{
	usart_data_in[usart_count]=UDR;
	if(usart_data_in[usart_count]==0x3B) 
	{
		usart_count=0;
		usart_data_in[usart_count]=0x3B;
	}
	if(usart_data_in[usart_count]==0xB3) 
	{
		TRANSMIT=0x01; // разрешение передачи
		for(int i=0;i<=usart_count;i++)
		{
			buffer[i]=usart_data_in[i];
		}
		usart_data_in_num=usart_count+1;
		buffer_start();
		usart_count=0;
	}
	usart_count++;
}

// прерывание по завершению передачи
ISR (USART_TXC_vect)
{
    if(TRANSMIT) buffer_start(); // начало передачи
}
// инициализация USART
void usart_init(void)
{
	// инициализация выводов
	USART_DDR&=~RX;
	USART_DDR|=TX;
	// запись значения скорости передачи
	UBRRL = LO(bauddivider);
	UBRRH = HI(bauddivider);
	// инициализация регистров управления
	UCSRA = 0;
	UCSRB = 1<<RXEN| // включить прием
			1<<TXEN| // включить передачу
			1<<RXCIE| // включить прерывание по приему
			1<<TXCIE; // включить прерывание по передаче
	UCSRC = 1<<URSEL| // запись в регистр UCSRC
			1<<UCSZ0| // размер данных 8 бит
			1<<UCSZ1;
}
// функция вывода буффера
void buffer_out(void)
{
	buffer_index++;	// увеличение индекса
	if(buffer_index==usart_data_in_num) // если вывели весь буффер
	{
		TRANSMIT=0x00;
		UCSRB &=~(1<<TXEN); // выключить передачу
		UCSRB &=~(1<<UDRIE); // запрещаем прерывание по опустошению - передача закончена
		UCSRA &=~(1<<TXC);	 // вручную сбрасываем флаг TXC - переход на прерывание по завершению передачи
	}
	else // продолжаем выводить буффер
	{
		UDR = buffer[buffer_index];	// берем данные из буффера. 
	}
}
// функция начала передачи буффера
void buffer_start(void)
{
	buffer_index=0;	   // сбрасываем индекс
	UCSRB|=(1<<TXEN); // включить передачу
	UDR = buffer[0];   // отправляем первый байт
	UCSRB|=(1<<UDRIE); // разрешаем прерывание UDRE	
}