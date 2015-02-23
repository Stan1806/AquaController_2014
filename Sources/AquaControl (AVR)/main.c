/* 	АКВАКОНТРОЛЛЕР 2014 В. 1.0.0.

	Организация управляющих подпрограмм, реализующих контроль над аквариумом.
	Опрос температурных датчиков, электродов PH, Redox, уровня воды и отработка сотвествующих режимов управления.
	Отсчет временных интервалов заданных таймеров и управление соответствующими каналами.
	Получение и отправка пакетов данных, запись настроек в энергонезависимую память.
	Реализация диспетчера задач, основанного на автомате флагов.
	
	Пустоводов С.А. 2014г.
*/

// стандартные библиотеки
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <stdlib.h> 
#include <string.h> 
#include <math.h>

#include "ds18b20.c" // работа с 1-Wire
 
#define baudrate 9600UL // скорость передачи USART
#define FREQ_CPU 11059200UL // частота МК
// определители ЖКИ
#define LCD_DATA_PORT PORTA 
#define LCD_DATA_DDR DDRA 
#define LCD_DATA_PIN PINA 
#define LCD_SYGNAL_PORT PORTC 
#define LCD_SYGNAL_DDR DDRC 
#define DATA_MASK 0x0F
#define RS _BV(PC7) 
#define E _BV(PC6) 
// определители I2C
#define I2C_PORT PORTD 
#define I2C_DDR DDRD 
#define I2C_PIN PIND 
#define SDA_N 2 
#define SDA _BV(PD2) 
#define SCL _BV(PD3) 
// определители ADC
#define ADC_DDR DDRA
#define ADC_chanel 0x07
// определители RELE
#define RELE_PORT_1 PORTD 
#define RELE_DDR_1 DDRD 
#define RELE_PORT_2 PORTC 
#define RELE_DDR_2 DDRC 
#define RELE_1 _BV(PD4) 
#define RELE_2 _BV(PD5) 
#define RELE_3 _BV(PD6) 
#define RELE_4 _BV(PD7) 
#define RELE_5 _BV(PC0) 
#define RELE_6 _BV(PC1) 
#define RELE_7 _BV(PC2) 
#define RELE_8 _BV(PC3) 
// определители LED
#define LCD_LED _BV(PC5) 
#define LED _BV(PC4) 
#define LED_PORT PORTC
#define LED_DDR DDRC 
#define LCD_LED_PORT PORTC
#define LCD_LED_DDR DDRC
// определители ADM690A
#define WDI_DDR DDRA
#define WDI_PORT PORTA
#define WDI _BV(PA3)

// глобальные переменные
unsigned char buffer_ds[3]; // время DS1307N (секунды, минуты, часы)
unsigned char buffer_time[3]; // установка времени DS1307N - синхронизация с ПК (секунды, минуты, часы)
unsigned char temp_control=0; // флаг температурного режима
unsigned char produv=0; // флаг режима продувки крышки
int set_temp=260; // задаваемая температура режима 
unsigned int cooler_set=64; // мощность кулеров (0-100)
unsigned char set_temp_dec=3; // температурный парог датчиков (десятые градуса)
int temp_now=0; // текущая температура
unsigned char heater=0; // флаг - нагрев воды
unsigned char cooler=0; // флаг - охлаждение воды
unsigned char co2=0; // флаг режима CO2/PH
unsigned char co2_set=0; // флаг - подача СО2
unsigned char rele_set[8]={0,0,0,0,0,0,0,0}; // утсановка RELE в ручном режиме
unsigned char hand_mode_set=0; // состояние RELE (ручной режим)
unsigned char time_one_sec=0; // отсчет секундного интервала
unsigned char doliv=0; // флаг режима долива воды
unsigned char pompa=0; // флаг - долить воды
int doliv_porog1=0; // порог отключения подачи воды
int doliv_porog2=0; // порог начала подачи воды
unsigned char WATER_ERROR=0; // неисправность датчика долива воды
unsigned char WATER_ERROR_POMPA=0; // неисправность датчика долива воды, фиксация при работе помпы
unsigned char pompa_time_count=0; // отсчет времени работы помпы
unsigned char water_lvl_status=0; // состояние помпы (0x30-отключено, 0x31-таймер, 0x32-ручной режим, 0x33-долив воды, 0x34-неисправность датчика уровня воды)
unsigned char co2_status=0; // состояне электроклапана (0x30-отключено, 0x31-таймер, 0x32-ручной режим, 0x33-режим PH/CO2)
unsigned char cooler_hand_start=0; // состояние кулеров при запуске устройства (ручной режим)
unsigned int cooler_hand_power=0; // мощность кулеров (заданная)

#include "i2c.h" // работа с I2C
#include "USART.h" // работа с USART

volatile unsigned int FlagsFastCount=0; // флаги системного счетчика
unsigned char temp_req=0; // выбор опрашваемого температурного датчика
unsigned long adc_data=0; // значение АЦП
unsigned char dh,dl; // значения регистров АЦП
unsigned long ph=0; // PH (считанное с ADC)
unsigned long redox=0; // redox-потенциал (считанное с ADC)
int water_lvl=0; // текущей уровень воды (считанное с ADC)
int water_lvl_last=0; // предыдущее значение уровня воды
int water_lvl_pompa=0; // предыдущее значение уровня воды, при включенной помпе
unsigned char water_lvl_ovf_time=0; // отсчет времени с момента резкого изменения уровня воды (исправность датчика)
long k1=1000; // коэффициент 1 для каллибровки PH-электрода
long k2=1000; // коэффициент 2 для каллибровки PH-электрода
long k=1000; // коэффициент для каллибровки redox-электрода
unsigned int adc_ph_1=0; // значение PH для первого раствора
unsigned int adc_ph_2=0; // значение PH для второго раствора
unsigned int adc_redox=0; // значение redox для каллибровочного раствора
unsigned int ph_high=700; // значение  PH, при котором включить подачу CO2 (7*100)
unsigned int ph_low=500; // значение  PH, при котором отключить подачу CO2 (5*100)
unsigned long adc_ph_1_calibr=0; // полученные каллибровочные значения раствора 1
unsigned long adc_ph_2_calibr=0; // полученные каллибровочные значения раствора 2
unsigned long adc_redox_calibr=0; // полученное каллибровочное значение redox
unsigned char adc_count=0; // отсчет опроса каналов ADC
float cooler_data[4]={0,0,0,0}; // значения ШИМ для режимов, использующих кулера
float cooler_min=64; // минимальная мощность кулеров (~25%)
unsigned char cooler_start_pulse[4]={0,0,0,0}; // отсчет интервалов времени для формирования стартовых импульсов кулеров
unsigned char temp_start_pulse=0; // отсчет интервала времени опроса температурных датчиков
// переменные ежедневных таймеров
volatile unsigned long timer1_start[9]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}; // начало Т1
volatile unsigned long timer1_stop[9]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}; // конец Т1
volatile unsigned long timer2_start[9]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}; // начало Т2
volatile unsigned long timer2_stop[9]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}; // конец Т2
volatile unsigned long timer3_start[9]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}; // начало Т3
volatile unsigned long timer3_stop[9]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}; // конец Т3 
volatile unsigned long time_min_and_sec=0; // значение минут и секунд в числовом представлении (0-5959) - для фиксации начала следующего часа
volatile unsigned long timer_produv_time=500; // время продувки кулерами (5*100) 
volatile unsigned char timer_produv_period=1; // периодичность включения продувки крышки
volatile unsigned char timer_produv_period_count=0; // отсчет пероида для продувки крышки
volatile unsigned long time_now=0; // значение текущего времени в числовом представлении (07h30m14s*10000=73014)
volatile unsigned long lcd_led_start=70000; // время включения подсветки ЖКИ (7*10000)
volatile unsigned long lcd_led_stop=230000; // время отключения подсветки ЖКИ (23*10000)
volatile unsigned long cooler_start=70000; // время включения кулеров (7*10000)
volatile unsigned long cooler_stop=230000; // время отключения кулеров (23*10000)
volatile unsigned long co2_start=70000; // время включения СО2 (7*10000)
volatile unsigned long co2_stop=230000; // время отключения СО2 (23*10000)
unsigned long data_ph=0; // значение PH с учетом каллибровочных коэффициентов
unsigned long data_redox=0; //значение redox с учетом каллибровочного коэффициента
unsigned char rele_ports_status=0; // состояние порта RELE 
unsigned char rele_ports_status_lcd=0; // отображение активности каналов на ЖКИ
unsigned char heater_timer_status=0; // флаг - нагреватель в режиме таймеров
unsigned char co2_timer_status=0; // флаг - СО2 в режиме таймеров
unsigned char doliv_timer_status=0; // флаг - помпа в режиме таймеров
unsigned char string_to_print=0; // отсчет отображаемых строк ЖКИ
unsigned char set_temp_sensor=1; // установленный температурный датчик (температурный режим)
unsigned char rele_hand_status=0; // состояние RELE (ручной режим)
unsigned char cooler_chanel_status=0; // состояние кулеров (0x30-отключено, 0x31-таймер, 0x32-ручной режим, 0x33-температурный режим, 0x34-продув)
unsigned char heater_chanel_status=0; // состояние нагревателя (0x30-отключено, 0x31-таймер, 0x32-ручной режим, 0x33-температурный режим)
unsigned char mode_status=0; // состояние режимов
unsigned char eeprom_count=0; // счетчик EEPROM
unsigned char EEPROM_WRITE=0; // флаг записи EEPROM

// прототипы функций
void scale_t (uint16_t t, uint16_t *c, uint16_t *d, uint8_t *s); // преобразование температуры
void set_temp_mode_and_produv(void); // температурный режим / режим продувки крышки
void save_option_eeprom(void); // сохранить настройки в EEPROM
void load_option_eeprom(void); // загрузить настройки из EEPROM
void timers_data_to_uart(void); // отправка данных таймеров
void events_data_to_uart(void); // отправки текущих данных режимов на ПК
void water_doliv(void); // режим атодолива воды

#include "hd44780.h" // подключение библиотеки для работы с HD44780

// обработчик прерывания - системный таймер (переполнение)
ISR (TIMER2_OVF_vect)
{
	TCNT2 = 31; // необходимо для получения интервала 1 секунда (0.333...*3=1)
    static uint16_t PrevState=0, FastCount=0;
	
	FastCount++;
	// определяем переход битов из 0 в 1 в пременной FastCount
	// и устанавливаем флаги в глобальной переменной FlagsFastCount, сбрасывать флаги
	// должен диспетчер, после вызова подпрограмм.
	FlagsFastCount|=~PrevState & FastCount;
	// записываем предыдущее состояние
	PrevState = FastCount;
} 

static inline void InitController(void) // инициализация контроллера
{	
	// отключить прерывания
	cli();	
	// конфигурация выводов	
	LCD_DATA_DDR|=0xF0;	
	LCD_SYGNAL_DDR|=0xC0;
	// ADC
	ADC_DDR&=~ADC_chanel;
	ADMUX=0b01000000;
	// включение модуля, выбор режима Free Running, делитель 8
	ADCSRA=(1<<ADEN)|(1<<ADATE)|(1<<ADPS0)|(1<<ADPS1);
	ADCSRA|=(1<<ADSC); // команда начала преобразований АЦП
	// RELE
	RELE_DDR_1 |=RELE_1|RELE_2|RELE_3|RELE_4;
	RELE_DDR_2 |=RELE_5|RELE_6|RELE_7|RELE_8;
	RELE_PORT_1&=~(RELE_1|RELE_2|RELE_3|RELE_4);
	RELE_PORT_2&=~(RELE_5|RELE_6|RELE_7|RELE_8);
	// LED
	LCD_LED_DDR |=LCD_LED;
	LED_DDR |=LED;
	LCD_LED_PORT|=LCD_LED;
	LED_PORT&=~LED;
	// WDI
	WDI_DDR|=WDI;
	WDI_PORT&=~WDI;
	// WDI стартовый импульс
	WDI_PORT|=WDI;
	_delay_us(100);
	WDI_PORT&=~WDI;
	// вывод PWM
	DDRB|= _BV(PB3);
	// инициализация ЖКИ
	_delay_ms(100);
	init_LCD();
	// системный таймер
	TIMSK = _BV(TOIE2); // разрешаем прерывания по переполнению таймера
	TCCR2 = _BV(CS21);	// устанавливаем предделение
	// Fast PWM, очистить OC0 при совпадении, предделение 8
	TCCR0 = (1<<CS01)|(1<<WGM01)|(1<<WGM00)|(1<<COM01)|(0<<COM00);
	OCR0  = 0;
	TCNT0 = 0;
    TCNT2 = 31; // необходимо для получения интервала 0.333 с	
	// включить прерывания
	sei();
}

// преобразование температуры
void scale_t (uint16_t t, uint16_t *c, uint16_t *d, uint8_t *s)
{
	uint16_t cel=0,dec=0; // переменные целых и десятичных значений числа
	uint8_t sig=0; // переменная знака числа
	
	if(t&0x8000) // отрицательная температура
	{
		cel=((t^0xffff)+1)>>4 ;
		dec=16-(t&0xf);
		sig='-';
	} 
	else // положительная температура
	{
		cel=t>>4;
		dec=t&0xf;
		sig='+';
	};

	if(t==0) sig=' '; // 0 значение
	dec*=625;
	
	*c=cel;	// Целое
	*d=dec;	// Десятичное
	*s=sig;	// Знак
}

int main(void)
{
	struct // данные температурных датчиков
	{
		uint8_t		s_p;	//	Sensor Present - перевоначальный ответ датчика
		uint8_t		s_m;	//	состояние
		uint16_t	m_t;	//	температура
	} ST[3]; // 3 датчика
	
	// преобразованные данные температуры
	uint16_t sb=0,mb=0;
	uint8_t s=0;

	// инициализация
	InitController(); // МК
	usart_init(); // USART
	DS1307_init(); // часы DS1307N

	temp_req=0; // выбор опрашваемого температурного датчика

	// если в EEPROM записаны настройки - считать
	if(eeprom_read_word((unsigned int*)1000)==0x1d) 
	{
		load_option_eeprom();
	}
	// если перед выключением кулер был выбран в ручном режиме
	if(cooler_start_pulse[0]>0) cooler_hand_start=1; // формировать стартовый импульс
	else cooler_hand_start=0;
	// для других режимов - обнулить стартовый импульс
	cooler_start_pulse[1]=0;
	cooler_start_pulse[2]=0;
	cooler_start_pulse[3]=0;
	
	while(1)
	{
		if (FlagsFastCount & _BV(7)) // ~ 41 ms
		{   
			// ----- поочередное отображение строк ЖКИ ----- //
			string_to_print++;
			switch(string_to_print)
			{
				case 1: string_to_LCD(1,0,string_1); break;
				case 2:	string_to_LCD(2,0,string_2); break;
				case 3:	string_to_LCD(3,0,string_3); break;
				case 4:	{string_to_LCD(4,0,string_4); string_to_print=0;} break;
				default:; break;
			}
			FlagsFastCount &= ~_BV(7); // сбросить флаг
		};

		if (FlagsFastCount & _BV(9)) // ~ 167 ms
		{	
			// ----- опрос температурных датчиков ----- //
			switch(temp_req)
			{
				case 0: // начать преобразование
					ST[0].s_p = DS18B20_start_meas( DS18X20_POWER_EXTERN, 'b', 4);
					ST[1].s_p = DS18B20_start_meas( DS18X20_POWER_EXTERN, 'b', 2);
					ST[2].s_p = DS18B20_start_meas( DS18X20_POWER_EXTERN, 'b', 1);
					temp_req =1;
				break;
				case 1:
					temp_start_pulse++;
					if(temp_start_pulse==20) // отсчет времени перед считыванием
					{
						ST[0].s_m = DS18B20_read_meas_single(&ST[0].m_t, 'b', 4);
						ST[1].s_m = DS18B20_read_meas_single(&ST[1].m_t, 'b', 2);
						ST[2].s_m = DS18B20_read_meas_single(&ST[2].m_t, 'b', 1);
						temp_req = 0;
						temp_start_pulse=0;
					}	
				break;
				default:; break;
			}
			// ----- преобразование температуры ----- //
			// 1-й датчик
			if(ST[0].s_m == DS18X20_OK) // датчик активен
			{
				sb=0;mb=0;s=' ';							
				scale_t(ST[0].m_t, &sb, &mb, &s); // преобразование					
				// запись в переменную для отображения
				string_2[5] = (((sb/10) % 10)+0x30);
				string_2[6] = ((sb % 10) + 0x30);			
				string_2[7] = ('.');						
				string_2[8] = (((mb/1000) % 10)+0x30);
			};
			// 2-й датчик
			if(ST[1].s_m == DS18X20_OK) // датчик активен
			{
				sb=0;mb=0;s=' ';							
				scale_t(ST[1].m_t, &sb, &mb, &s); // преобразование					
				// запись в переменную для отображения
				string_3[5] = (((sb/10) % 10)+0x30);
				string_3[6] = ((sb % 10) + 0x30);		
				string_3[7] = ('.');										
				string_3[8] = (((mb/1000) % 10)+0x30);					
			};
			// 3-й датчик
			if(ST[2].s_m == DS18X20_OK) // датчик активен
			{
				sb=0;mb=0;s=' ';							
				scale_t(ST[2].m_t, &sb, &mb, &s); // преобразование				
				// запись в переменную для отображения
				string_4[5] = (((sb/10) % 10)+0x30);
				string_4[6] = ((sb % 10) + 0x30);		
				string_4[7] = ('.');										
				string_4[8] = (((mb/1000) % 10)+0x30);					
			};
            
			// ----- текущее время ----- //
            DS1307(); // опрос DS1307N
			// преобразование и запись в переменную для отображения
			// часы
			string_1[0]=((((buffer_ds[2]&0xF0)>>4)*10)+(buffer_ds[2]&0x0F))/10+0x30; // HEX to BIN, вывод на LCD
			string_1[1]=((((buffer_ds[2]&0xF0)>>4)*10)+(buffer_ds[2]&0x0F))%10+0x30;
			string_1[2]=0x3A; 
			// минуты
			string_1[3]=((((buffer_ds[1]&0xF0)>>4)*10)+(buffer_ds[1]&0x0F))/10+0x30; // HEX to BIN, вывод на LCD
			string_1[4]=((((buffer_ds[1]&0xF0)>>4)*10)+(buffer_ds[1]&0x0F))%10+0x30;
			string_1[5]=0x3A; 
			// секунды
			string_1[6]=((((buffer_ds[0]&0xF0)>>4)*10)+(buffer_ds[0]&0x0F))/10+0x30; // HEX to BIN, вывод на LCD
			string_1[7]=((((buffer_ds[0]&0xF0)>>4)*10)+(buffer_ds[0]&0x0F))%10+0x30;
				
			// ----- опрос линий ADC ----- //
			adc_count++;
			// чтение регистров, хранящих оцифрованное значение
			dl=ADCL;
			dh=ADCH;
			adc_data =(dh<<8)+dl; // общее оцифрованное значение
			// переключение каналов
			if(adc_count==1)	
			{
				ph=adc_data; // значение PH
				ADMUX=0b01000001;
				ADCSRA|=(1<<ADSC); // команда начала преобразований АЦП
			}
			if(adc_count==2)	
			{
				redox=adc_data; // значение redox
				ADMUX=0b01000010;
				ADCSRA|=(1<<ADSC); // команда начала преобразований АЦП
			}
			if(adc_count==3)	
			{
				water_lvl=adc_data; // значение уровня воды
				ADMUX=0b01000000;
				ADCSRA|=(1<<ADSC); // команда начала преобразований АЦП
				adc_count=0;
			}
			
			// ----- преобразование и запись в переменные для отображение ----- //
			// PH, с учетом каллибровочных коэффицинтов
			data_ph=(ph*k1+k2)/1000;
			string_2[15]=data_ph/1000+0x30; 
			string_2[16]=(data_ph%1000)/100+0x30; 
			string_2[17]='.';
			string_2[18]=((data_ph%1000)%100)/10+0x30; 
			string_2[19]=((data_ph%1000)%100)%10+0x30; 
			// redox, с учетом каллибровочного коэффицинта
			data_redox=redox*k/1000;
			string_3[15]=data_redox/1000+0x30; 
			string_3[16]='.';
			string_3[17]=(data_redox%1000)/100+0x30; 
			string_3[18]=((data_redox%1000)%100)/10+0x30; 
			string_3[19]=((data_redox%1000)%100)%10+0x30; 
            
			FlagsFastCount &= ~_BV(9); // сбросить флаг
		};	
		
		if (FlagsFastCount & _BV(10)) // ~ 333 ms
		{
			// ----- обработка входящих данных ----- //
			// ----- данные передаются в виде строки ----- //
			if(usart_data_in[0]==0x3B) // принято начало пакета данных
			{
				switch(usart_data_in[1]) // определение команды
				{
					// ----- данные вкладки <Отображение информации> AquaControl ----- //
					
					case 0x01: // ручной режим, временные ограничения
					{
						// ручное управление каналами
						if(usart_data_in[22]==0xB3) // принят конец пакета данных
						{
							if(usart_data_in[2]==0x31) rele_set[0]=1; // освещение 1
							else rele_set[0]=0;
							if(usart_data_in[3]==0x31) rele_set[1]=1; // освещение 2
							else rele_set[1]=0;
							if(usart_data_in[4]==0x31) rele_set[2]=1; // освещение 3
							else rele_set[2]=0;
							if(usart_data_in[5]==0x31) rele_set[3]=1; // освещение 4
							else rele_set[3]=0;
							if(usart_data_in[6]==0x31) rele_set[4]=1; // компрессор
							else rele_set[4]=0;
							if(usart_data_in[7]==0x31) // --- нагреватель
							{
								rele_set[5]=1; // нагреватель включить
								heater_chanel_status=0x32; // флаг - нагреватель в ручном режиме
							}
							else 
							{
								rele_set[5]=0; // нагреватель отключить
								heater_chanel_status=0; // сбросить флаг
							}
							if(usart_data_in[8]==0x31) // --- клапан СО2
							{
								rele_set[6]=1; // клапан СО2 включить
								co2_status=0x32; // флаг - электроклапан в ручном режиме
							}
							else 
							{
								rele_set[6]=0; // клапан СО2 отключить
								co2_status=0; // сбросить флаг
							}
							if(usart_data_in[9]==0x31) // --- помпа
							{
								rele_set[7]=1; // помпу включить
								water_lvl_status=0x32; // флаг - помпа в ручном режиме
							}
							else 
							{
								rele_set[7]=0; // помпу отключить
								water_lvl_status=0; // сбросить флаг
							}
							usart_data_in[22]=0; // пакет данных обработан
						}
							
						// ручное управление кулерами
						if(usart_data_in[10]==0x31)
						{
							cooler_start_pulse[0]++; // отсчет времени стартового импульса
							// ~3.33 с - устанавливаем заданную мощность
							if(cooler_start_pulse[0]==10) 
							{
								// получение значения мощности кулеров
								cooler_data[0]=(usart_data_in[11]*100+usart_data_in[12]*10+usart_data_in[13])*2.55;
								cooler_start_pulse[0]=9; // продолжаем выполнять условие
								// если мощности кулеров меньше минимальной
								if(cooler_data[0]<cooler_min) cooler_data[0]=cooler_min;
								// использовать текущую заданную мощность для режима таймеров
								cooler_data[2]=cooler_data[0];
							}
							// мощность кулеров на максимум
							else cooler_data[0]=255;
							OCR0=cooler_data[0]; // задать PWM
							cooler_chanel_status=0x32; // флаг - кулера в ручном режиме
						}
						else 
						{
							// сбросить PWM
							OCR0=0; 
							cooler_start_pulse[0]=0;
							cooler_chanel_status=0;
							cooler_hand_start=0;
						}
						
						// установить временные интервалы для подсветки ЖКИ и кулеров (числовое представление данных)
						lcd_led_stop=(((unsigned long)usart_data_in[14]-0x30)*10+((unsigned long)usart_data_in[15]-0x30))*10000;
						lcd_led_start=(((unsigned long)usart_data_in[16]-0x30)*10+((unsigned long)usart_data_in[17]-0x30))*10000;
						cooler_stop=(((unsigned long)usart_data_in[18]-0x30)*10+((unsigned long)usart_data_in[19]-0x30))*10000;
						cooler_start=(((unsigned long)usart_data_in[20]-0x30)*10+((unsigned long)usart_data_in[21]-0x30))*10000;	
					}break;
					
					case 0x02: // синхронизация часов DS1307N с ПК
					{
						// получение времени с ПК
						buffer_time[0]=(usart_data_in[2]-0x30)*10+(usart_data_in[3]-0x30); // часы
						buffer_time[1]=(usart_data_in[4]-0x30)*10+(usart_data_in[5]-0x30); // минуты
						buffer_time[2]=(usart_data_in[6]-0x30)*10+(usart_data_in[7]-0x30); // секунды
						
						DS1307_set_time(); // запись в часы
						if(usart_data_in[8]==0xb3) 
						{
							// выйти из обработки
							usart_data_in[8]=0x00; 
							usart_data_in[1]=0x00; 
						}
					}break;
					
					case 0x13: // сохранить настройки в EEPROM
					{
						eeprom_count=0;
						EEPROM_WRITE=1;
						if(usart_data_in[2]==0xb3) 
						{
							// выйти из обработки
							usart_data_in[2]=0x00; 
							usart_data_in[1]=0x00; 
						}
					}break;
					
					// ----- данные вкладки <Режимы/События> AquaControl ----- //
					
					case 0x03: // установить температурный режим
					{
						temp_control=1; // флаг - установить температурный режим
						// получение данных режима 
						set_temp_sensor=usart_data_in[2]-0x30; // выбор датчика 
						set_temp=((usart_data_in[3]-0x30)*10+(usart_data_in[4]-0x30))*10; // заданная температура
						set_temp_dec=usart_data_in[5]-0x30; // порог срабатывания 
						cooler_set=((usart_data_in[6]-0x30)*100+(usart_data_in[7]-0x30)*10+(usart_data_in[8]-0x30))*2.55; // мошность кулеров
						if(usart_data_in[9]==0xb3) 
						{
							// выйти из обработки
							usart_data_in[9]=0x00; 
							usart_data_in[1]=0x00; 
						}
					}break;
					
					case 0x04: // отключить температурный режим 
					{
						temp_control=2; // флаг - отключить температурный режим
						// сбросить переменные
						cooler=0; // кулера
						heater=0; // нагреватель
						cooler_start_pulse[1]=0; // стартовый импульс
						if(usart_data_in[2]==0xb3) 
						{
							// выйти из обработки
							usart_data_in[2]=0x00; 
							usart_data_in[1]=0x00; 
						}
					}break;
					
					case 0x06: // установить режим продувки крышки
					{
						produv=1; // флаг - продувать крышку
						// получение времени продувки
						timer_produv_time=(((unsigned long)usart_data_in[2]-0x30)*10+((unsigned long)usart_data_in[3]-0x30))*100
										+((unsigned long)usart_data_in[4]-0x30)*10+((unsigned long)usart_data_in[5]-0x30);
						// получение периода включения продувки
						timer_produv_period=usart_data_in[6]-0x30;
						// получение минимального значения мощности кулеров
						cooler_min=((usart_data_in[7]-0x30)*100+(usart_data_in[8]-0x30)*10+(usart_data_in[9]-0x30))*2.55;
						if(usart_data_in[10]==0xb3) 
						{
							// выйти из обработки
							usart_data_in[10]=0x00; 
							usart_data_in[1]=0x00; 
						}
					}break;
					
					case 0x07: // отключить режим продувки крышки
					{
						produv=2; // флаг - отключить продувку крышки
						cooler_start_pulse[3]=0; // сбросить стартовый импульс
						if(usart_data_in[2]==0xb3) 
						{
							// выйти из обработки
							usart_data_in[2]=0x00; 
							usart_data_in[1]=0x00; 
						}
					}break;
					
					case 0x11: // установить режим СО2/РН
					{
						co2=1; // флаг - установить режим CO2/PH
						// получить данные режима
						// порог включения СО2
						ph_high=(usart_data_in[2]-0x30)*100+(usart_data_in[3]-0x30)*10+(usart_data_in[4]-0x30);
						// порог отключения СО2
						ph_low=(usart_data_in[5]-0x30)*100+(usart_data_in[6]-0x30)*10+(usart_data_in[7]-0x30);
						// временные ограничения СО2
						co2_stop=(((unsigned long)usart_data_in[8]-0x30)*10+((unsigned long)usart_data_in[9]-0x30))*10000;
						co2_start=(((unsigned long)usart_data_in[10]-0x30)*10+((unsigned long)usart_data_in[11]-0x30))*10000;
						if(usart_data_in[12]==0xb3) 
						{
							// выйти из обработки
							usart_data_in[12]=0x00; 
							usart_data_in[1]=0x00; 
						}
					}break;
					
					case 0x12: // отключить режим СО2/РН
					{
						co2=0; // флаг - отключить режим СО2/РН
						// если канал не задан таймерами - отключить электроклапан
						if(co2_timer_status==0) RELE_PORT_2&=~RELE_7;
						if(usart_data_in[2]==0xb3) 
						{
							// выйти из обработки
							usart_data_in[2]=0x00; 
							usart_data_in[1]=0x00; 
						}
					}break;
					
					case 0x15: // установить режим автоматического долива воды
					{
						doliv=1; // флаг - установить автодолив
						water_lvl_last=water_lvl; // обновить предыдущее значение уровня воды
						WATER_ERROR_POMPA=0;
						if(usart_data_in[2]==0xb3) 
						{
							// выйти из обработки
							usart_data_in[2]=0x00; 
							usart_data_in[1]=0x00; 
						}
					}break;
					
					case 0x16: // отключить режим автоматического долива воды
					{
						pompa=0;
						doliv=2; // флаг - отключить автодолив
						if(usart_data_in[2]==0xb3) 
						{
							// выйти из обработки
							usart_data_in[2]=0x00; 
							usart_data_in[1]=0x00; 
						}
					}break;
					
					case 0x17: // установить порог 1 датчика автодолива
					{
						doliv_porog1=water_lvl; // считать значение
						if(usart_data_in[2]==0xb3) 
						{
							// выйти из обработки
							usart_data_in[2]=0x00; 
							usart_data_in[1]=0x00; 
						}
					}break;
					
					case 0x18: // установить порог 2 датчика автодолива
					{
						doliv_porog2=water_lvl; // считать значение
						if(usart_data_in[2]==0xb3) 
						{
							// выйти из обработки
							usart_data_in[2]=0x00; 
							usart_data_in[1]=0x00; 
						}
					}break;
					
					case 0x19: // передача текущих данных исполняемых режимов
					{
						events_data_to_uart(); // отправить значения
						if(usart_data_in[2]==0xb3) 
						{
							// выйти из обработки
							usart_data_in[2]=0x00; 
							usart_data_in[1]=0x00; 
						}
					}break;
					
					// ----- данные вкладки <Ежедневные таймеры> AquaControl ----- //
					
					case 0x05: // установить значение таймеров
					{
						for(int i=0;i<9;i++) // для 9 каналов (включая PWM-канал)
						{
							// значения времени преобразуются в числовое значение
							timer1_start[i]=(((unsigned long)usart_data_in[2+36*i]-0x30)*10+((unsigned long)usart_data_in[3+36*i]-0x30))*10000
										+(((unsigned long)usart_data_in[4+36*i]-0x30)*10+((unsigned long)usart_data_in[5+36*i]-0x30))*100
										+(((unsigned long)usart_data_in[6+36*i]-0x30)*10+((unsigned long)usart_data_in[7+36*i]-0x30));
							timer1_stop[i]=(((unsigned long)usart_data_in[8+36*i]-0x30)*10+((unsigned long)usart_data_in[9+36*i]-0x30))*10000
										+(((unsigned long)usart_data_in[10+36*i]-0x30)*10+((unsigned long)usart_data_in[11+36*i]-0x30))*100
										+(((unsigned long)usart_data_in[12+36*i]-0x30)*10+((unsigned long)usart_data_in[13+36*i]-0x30));
							timer2_start[i]=(((unsigned long)usart_data_in[14+36*i]-0x30)*10+((unsigned long)usart_data_in[15+36*i]-0x30))*10000
										+(((unsigned long)usart_data_in[16+36*i]-0x30)*10+((unsigned long)usart_data_in[17+36*i]-0x30))*100
										+(((unsigned long)usart_data_in[18+36*i]-0x30)*10+((unsigned long)usart_data_in[19+36*i]-0x30));
							timer2_stop[i]=(((unsigned long)usart_data_in[20+36*i]-0x30)*10+((unsigned long)usart_data_in[21+36*i]-0x30))*10000
										+(((unsigned long)usart_data_in[22+36*i]-0x30)*10+((unsigned long)usart_data_in[23+36*i]-0x30))*100
										+(((unsigned long)usart_data_in[24+36*i]-0x30)*10+((unsigned long)usart_data_in[25+36*i]-0x30));
							timer3_start[i]=(((unsigned long)usart_data_in[26+36*i]-0x30)*10+((unsigned long)usart_data_in[27+36*i]-0x30))*10000
										+(((unsigned long)usart_data_in[28+36*i]-0x30)*10+((unsigned long)usart_data_in[29+36*i]-0x30))*100
										+(((unsigned long)usart_data_in[30+36*i]-0x30)*10+((unsigned long)usart_data_in[31+36*i]-0x30));
							timer3_stop[i]=(((unsigned long)usart_data_in[32+36*i]-0x30)*10+((unsigned long)usart_data_in[33+36*i]-0x30))*10000
										+(((unsigned long)usart_data_in[34+36*i]-0x30)*10+((unsigned long)usart_data_in[35+36*i]-0x30))*100
										+(((unsigned long)usart_data_in[36+36*i]-0x30)*10+((unsigned long)usart_data_in[37+36*i]-0x30));
						}
						if(usart_data_in[326]==0xb3) 
						{
							// выйти из обработки
							usart_data_in[326]=0x00; 
							usart_data_in[1]=0x00; 
						}
					}break;
					
					case 0x14: // отправить данные таймеров на ПК
					{
						timers_data_to_uart();
						if(usart_data_in[2]==0xb3) 
						{
							// выйти из обработки
							usart_data_in[2]=0x00; 
							usart_data_in[1]=0x00; 
						}
					}break;
					
					// ----- данные вкладки <Каллибровка электродов> AquaControl ----- //
					
					case 0x08: // считать значение 1-го калибрующего раствора
					{
						adc_ph_1=ph; // получить значение ADC 1-го раствора
						// получить соответствующие номиналы PH каллибровочных растворов
						adc_ph_1_calibr=((unsigned long)usart_data_in[2]-0x30)*1000+((unsigned long)usart_data_in[3]-0x30)*100
										+((unsigned long)usart_data_in[4]-0x30)*10+((unsigned long)usart_data_in[5]-0x30);
						adc_ph_2_calibr=((unsigned long)usart_data_in[6]-0x30)*1000+((unsigned long)usart_data_in[7]-0x30)*100
										+((unsigned long)usart_data_in[8]-0x30)*10+((unsigned long)usart_data_in[9]-0x30);
						if(usart_data_in[13]==0xb3) 
						{
							// выйти из обработки
							usart_data_in[13]=0x00; 
							usart_data_in[1]=0x00; 
						}
					}break;
					
					case 0x09: // считать значение 2-го калибрующего раствора
					{
						adc_ph_2=ph; // получить значение ADC 2-го раствора
						// рассчитать коэффициенты PH (уравнение прямой, проходящей через 2 точки)
						// для удобства расчетов используется приведение значений к целому виду (7010 = 7.01 * 1000 PH)
						// в последующем, для режимов работы и отображения используется обратное приведение значений
						
						// пропорциональный коэффициент:
						k1=((long)adc_ph_1_calibr-(long)adc_ph_2_calibr)*1000/((long)adc_ph_1-(long)adc_ph_2);
						// коэффициент смещения:
						k2=(long)adc_ph_1_calibr*1000-k1*adc_ph_1;
						if(usart_data_in[13]==0xb3) 
						{
							// выйти из обработки
							usart_data_in[13]=0x00; 
							usart_data_in[1]=0x00; 
						}
					}break;
					
					case 0x10: // считать значение калибрующего раствора redox
					{
						adc_redox=redox; // получить значение ADC раствора
						// получить соответствующий номинал каллибрующего раствора
						adc_redox_calibr=(usart_data_in[10]-0x30)*100+(usart_data_in[11]-0x30)*10+(usart_data_in[12]-0x30);
						// рассчитать коэффициент
						k=(unsigned long)adc_redox_calibr*1000/adc_redox;
						if(usart_data_in[13]==0xb3) 
						{
							// выйти из обработки
							usart_data_in[13]=0x00; 
							usart_data_in[1]=0x00; 
						}
					}break;
					default:; break;
				}
			}
			
			// преобразование текущего времени в числовое значение (часы, минуты, секунды)
			time_now=(((unsigned long)string_1[0]-0x30)*10+((unsigned long)string_1[1]-0x30))*10000
					+(((unsigned long)string_1[3]-0x30)*10+((unsigned long)string_1[4]-0x30))*100
					+(((unsigned long)string_1[6]-0x30)*10+((unsigned long)string_1[7]-0x30));	
			// преобразование текущего времени в числовое значение (минуты, секунды) - для режима продувки кулеров
			time_min_and_sec=(((unsigned long)string_1[3]-0x30)*10+((unsigned long)string_1[4]-0x30))*100
							 +(((unsigned long)string_1[6]-0x30)*10+((unsigned long)string_1[7]-0x30));
				
			// ----- управление каналами RELE ----- //
			LED_PORT&=~LED; // отключить светодиод	
			time_one_sec++; // отсчет временного интервала
			if(time_one_sec==3) // 1 c - управление каналами
			{
				// если текущее время находится в промежутке заданных интервалов таймеров
				// или задан ручной режим работы канала
				// активировать канал
				// иначе - отключить канал
				
				if (((time_now>=timer1_start[0])&&(time_now<timer1_stop[0])&&(timer1_start[0]!=timer1_stop[0]))||
					((time_now>=timer2_start[0])&&(time_now<timer2_stop[0])&&(timer2_start[0]!=timer2_stop[0]))||
					((time_now>=timer3_start[0])&&(time_now<timer3_stop[0])&&(timer3_start[0]!=timer3_stop[0]))||
					(rele_set[0]==1)) RELE_PORT_1|=RELE_1;
				else {RELE_PORT_1&=~RELE_1; rele_set[0]=0;}
				
				if (((time_now>=timer1_start[1])&&(time_now<timer1_stop[1])&&(timer1_start[1]!=timer1_stop[1]))||
					((time_now>=timer2_start[1])&&(time_now<timer2_stop[1])&&(timer2_start[1]!=timer2_stop[1]))||
					((time_now>=timer3_start[1])&&(time_now<timer3_stop[1])&&(timer3_start[1]!=timer3_stop[1]))||
					(rele_set[1]==1)) RELE_PORT_1|=RELE_2;
				else {RELE_PORT_1&=~RELE_2; rele_set[1]=0;}
				
				if (((time_now>=timer1_start[2])&&(time_now<timer1_stop[2])&&(timer1_start[2]!=timer1_stop[2]))||
					((time_now>=timer2_start[2])&&(time_now<timer2_stop[2])&&(timer2_start[2]!=timer2_stop[2]))||
					((time_now>=timer3_start[2])&&(time_now<timer3_stop[2])&&(timer3_start[2]!=timer3_stop[2]))||
					(rele_set[2]==1)) RELE_PORT_1|=RELE_3;
				else {RELE_PORT_1&=~RELE_3; rele_set[2]=0;}
				
				if (((time_now>=timer1_start[3])&&(time_now<timer1_stop[3])&&(timer1_start[3]!=timer1_stop[3]))||
					((time_now>=timer2_start[3])&&(time_now<timer2_stop[3])&&(timer2_start[3]!=timer2_stop[3]))||
					((time_now>=timer3_start[3])&&(time_now<timer3_stop[3])&&(timer3_start[3]!=timer3_stop[3]))||
					(rele_set[3]==1)) RELE_PORT_1|=RELE_4;
				else {RELE_PORT_1&=~RELE_4; rele_set[3]=0;}
				
				if (((time_now>=timer1_start[4])&&(time_now<timer1_stop[4])&&(timer1_start[4]!=timer1_stop[4]))||
					((time_now>=timer2_start[4])&&(time_now<timer2_stop[4])&&(timer2_start[4]!=timer2_stop[4]))||
					((time_now>=timer3_start[4])&&(time_now<timer3_stop[4])&&(timer3_start[4]!=timer3_stop[4]))||
					(rele_set[4]==1)) RELE_PORT_2|=RELE_5;
				else {RELE_PORT_2&=~RELE_5; rele_set[4]=0;}
				
				if (((time_now>=timer1_start[5])&&(time_now<timer1_stop[5])&&(timer1_start[5]!=timer1_stop[5]))||
					((time_now>=timer2_start[5])&&(time_now<timer2_stop[5])&&(timer2_start[5]!=timer2_stop[5]))||
					((time_now>=timer3_start[5])&&(time_now<timer3_stop[5])&&(timer3_start[5]!=timer3_stop[5]))||
					(rele_set[5]==1)) 
					{
						if(!temp_control) RELE_PORT_2|=RELE_6; 
						heater_timer_status=1; // флаг - режим таймеров
						// если отключен температурный и ручной режимы
						if((heater_chanel_status!=0x32)&&(heater_chanel_status!=0x33)) heater_chanel_status=0x31; // флаг - режим таймеров
					} 
				else {RELE_PORT_2&=~RELE_6; rele_set[5]=0; heater_timer_status=0;}
				
				if (((time_now>=timer1_start[6])&&(time_now<timer1_stop[6])&&(timer1_start[6]!=timer1_stop[6]))||
					((time_now>=timer2_start[6])&&(time_now<timer2_stop[6])&&(timer2_start[6]!=timer2_stop[6]))||
					((time_now>=timer3_start[6])&&(time_now<timer3_stop[6])&&(timer3_start[6]!=timer3_stop[6]))||
					(rele_set[6]==1)) 
					{
						if(!co2) RELE_PORT_2|=RELE_7; 
						co2_timer_status=1; // флаг - режим таймеров
						// если отключены режимы PH/CO2 и ручной режим
						if((co2_status!=0x32)&&(co2_status!=0x33)) co2_status=0x31; // флаг - режим таймеров
					} 
				else {RELE_PORT_2&=~RELE_7; rele_set[6]=0; co2_timer_status=0;}
				
				if (((time_now>=timer1_start[7])&&(time_now<timer1_stop[7])&&(timer1_start[7]!=timer1_stop[7]))||
					((time_now>=timer2_start[7])&&(time_now<timer2_stop[7])&&(timer2_start[7]!=timer2_stop[7]))||
					((time_now>=timer3_start[7])&&(time_now<timer3_stop[7])&&(timer3_start[7]!=timer3_stop[7]))||
					(rele_set[7]==1)) 
					{
						if(!doliv) RELE_PORT_2|=RELE_8; 
						doliv_timer_status=1; // флаг - режим таймеров
						// если отключены режимы автодолив и ручной режим
						if((water_lvl_status!=0x32)&&(water_lvl_status!=0x33)) water_lvl_status=0x31; // флаг - режим таймеров
					}
				else {RELE_PORT_2&=~RELE_8; rele_set[7]=0; doliv_timer_status=0;}
				
				time_one_sec=0; // сбросить отсчет
                LED_PORT|=LED; // включить светодиод	
			}
			
			// ----- управление каналом PWM ----- //		
			// если текущее время находится в промежутке заданного временного интервала работы кулеров
			// если текущее время находится в промежутке заданных интервалов таймеров
			// если перед сбросом был задан ручной режим
			// активировать канал
			// иначе - отключить канал
			if((time_now>cooler_start)&&(time_now<cooler_stop))
			{
				if (((time_now>=timer1_start[8])&&(time_now<timer1_stop[8])&&(timer1_start[8]!=timer1_stop[8]))||
					((time_now>=timer2_start[8])&&(time_now<timer2_stop[8])&&(timer2_start[8]!=timer2_stop[8]))||
					((time_now>=timer3_start[8])&&(time_now<timer3_stop[8])&&(timer3_start[8]!=timer3_stop[8]))||
					(cooler_hand_start))
					{
						cooler_start_pulse[2]++; // отсчет времени стартового импульса
						// ~3.33 с - устанавливаем заданную мощность
						if(cooler_start_pulse[2]==10) 
						{
							// использовать заданную мощность ручного режима для режима таймеров
							cooler_data[2]=cooler_data[0];
							cooler_start_pulse[2]=9; // продолжаем выполнять условие
							// если мощности кулеров меньше минимальной
							if(cooler_data[2]<cooler_min) cooler_data[2]=cooler_min;
						}
						// мощность кулеров на максимум
						else cooler_data[2]=255;
						OCR0=cooler_data[2];
						// если отключены режимы продувки крышки и ручной режим
						if((cooler_chanel_status!=0x32)&&(cooler_chanel_status!=0x34)) cooler_chanel_status=0x31; // флаг - режим таймеров
					}	
				else 
				{	
					// сбросить PWM
					cooler_start_pulse[2]=0;
					// установка значения PWM относительно приоритета режимов
					if(cooler_start_pulse[0]) OCR0=cooler_data[0]; // 1-ручной режим управления
					if(cooler_start_pulse[3]) OCR0=cooler_data[3]; // 2-режим продувки крышки
					if(cooler_start_pulse[1]) OCR0=cooler_data[1]; // 3-температурный режим
					// если ни один из режимов не запущен - отключить PWM
					if((cooler_start_pulse[0]==0)&(cooler_start_pulse[1]==0)&(cooler_start_pulse[2]==0)&(cooler_start_pulse[3]==0)) OCR0=0;
				}
			}
			
			// ----- режим СО2/РН ----- //
			// если текущее время находится в промежутке заданного временного интервала работы
			if((time_now>co2_start)&&(time_now<co2_stop))
			{
				if(co2) // режим задан
				{
					if(data_ph>=ph_high) co2_set=1; // фиксация включения
					if(data_ph<=ph_low) co2_set=0; // фиксация отключения
					if(co2_set) 
					{
						co2_status=0x33;
						RELE_PORT_2|=RELE_7; // включить электроклапан
					}
					else 
					{
						RELE_PORT_2&=~RELE_7; // отключить электроклапан
						if(co2_timer_status) co2_status=0x31;
						if(rele_set[6]) co2_status=0x32;
						// если не задан режим таймера и ручной режим - отключить канал
						if((co2_timer_status==0)&(rele_set[6]==0)) co2_status=0x30;
					}
				}
				else 
				{
					if(rele_set[6]==1) co2_status=0x32;
					// если не задан режим таймера и ручной режим - отключить канал
					if((co2_timer_status==0)&(rele_set[6]==0)) {RELE_PORT_2&=~RELE_7; co2_status=0x30;}
				}
			}
			
			// ----- временной интервал работы подсветки ЖКИ ----- //
			// если текущее время находится в промежутке заданного временного интервала работы
			if((time_now>lcd_led_start)&&(time_now<lcd_led_stop)) LCD_LED_PORT&=~LCD_LED; // включить подсветку
			else LCD_LED_PORT|=LCD_LED; // выключить подсветку 
			
			// ----- обработка температурного режима и режима продувки крышки ----- //
			set_temp_mode_and_produv();
			
			// ----- обработка режима автоматического долива воды ----- //
			water_doliv();
			
			// ----- запись параметров в EEPROM ----- //
			if(EEPROM_WRITE) save_option_eeprom();
			
			// ----- состояния выводов, режимов ----- //
			rele_ports_status=(PIND>>4)+((PINC&0x01)<<4); // состояние порта RELE
			rele_ports_status_lcd=(PIND>>4)+(PINC<<4); // отображение активности каналов на ЖКИ
			rele_hand_status=rele_set[0]|(rele_set[1]<<1)|(rele_set[2]<<2)|(rele_set[3]<<3)|(rele_set[4]<<4);
			// состояние PWM
			if((cooler_start_pulse[0]==0)&(cooler_start_pulse[1]==0)&(cooler_start_pulse[2]==0)&(cooler_start_pulse[3]==0)) cooler_chanel_status=0x30;
			// состояние электроклапана
			if((rele_set[6]==0)&(co2_timer_status==0)&(co2==0)) co2_status=0x30;
			// состояние помпы
			if((rele_set[7]==0)&(doliv_timer_status==0)&(pompa==0)&(WATER_ERROR==0)) water_lvl_status=0x30;
			// состояние нагревателя
			if((rele_set[5]==0)&(heater_timer_status==0)&(heater==0)) heater_chanel_status=0x30;
			// состояния режимов
			if(temp_control==1) mode_status|=_BV(0);
			else mode_status&=~_BV(0);
			if(produv==1) mode_status|=_BV(1);
			else mode_status&=~_BV(1);
			if(co2==1) mode_status|=_BV(2);
			else mode_status&=~_BV(2);
			if(doliv==1) mode_status|=_BV(3);
			else mode_status&=~_BV(3);
			
			// если один из каналов в ручном режиме
			hand_mode_set=0;
			for(unsigned int i=0;i<8;i++)
			{
				if((rele_set[i]!=0)|(cooler_start_pulse[0]!=0)) hand_mode_set=1;
			}
			if(WATER_ERROR) // если датчик уровня воды не исправен - отображать информацию
			{
				string_1[11]='!';
				string_1[12]='E';
				string_1[13]='R';
				string_1[14]='R';
				string_1[15]='O';
				string_1[16]='R';
				string_1[17]='!';
				string_1[18]=0x20;
			}
			else
			{
				if(hand_mode_set) // если выбран ручной режим одного из каналов - отображать информацию
				{
					string_1[11]='!';
					string_1[12]='!';
					string_1[13]='H';
					string_1[14]='A';
					string_1[15]='N';
					string_1[16]='D';
					string_1[17]='!';
					string_1[18]='!';
				}
				else // иначе - указывать состояние большинства выводов - режим таймеров
				{
					string_1[11]='T';
					string_1[12]='I';
					string_1[13]='M';
					string_1[14]='E';
					string_1[15]='R';
					string_1[16]='S';
					string_1[17]=0x20;
					string_1[18]=0x20;
				}
			}
			
			// отображение активных каналов в текущий момент
			if((rele_ports_status_lcd&0x01)==0x01) string_4[11]=0x31;
			else string_4[11]='-';
			if((rele_ports_status_lcd&0x02)==0x02) string_4[12]=0x32;
			else string_4[12]='-';
			if((rele_ports_status_lcd&0x04)==0x04) string_4[13]=0x33;
			else string_4[13]='-';
			if((rele_ports_status_lcd&0x08)==0x08) string_4[14]=0x34;
			else string_4[14]='-';
			if((rele_ports_status_lcd&0x10)==0x10) string_4[15]=0x35;
			else string_4[15]='-';
			if((rele_ports_status_lcd&0x20)==0x20) string_4[16]=0x36;
			else string_4[16]='-';
			if((rele_ports_status_lcd&0x40)==0x40) string_4[17]=0x37;
			else string_4[17]='-';
			if((rele_ports_status_lcd&0x80)==0x80) string_4[18]=0x38;
			else string_4[18]='-';
			if(OCR0>0) string_4[19]='C';
			else string_4[19]='-';
			
			FlagsFastCount &= ~_BV(10); // сбросить флаг
		};
		
		if (FlagsFastCount & _BV(12)) // ~ 1,34 s
		{
			// формирование сигнала для ADM690A
			WDI_PORT|=WDI;
			_delay_us(100);
			WDI_PORT&=~WDI;
			
			FlagsFastCount &= ~_BV(12); // сбросить флаг
		};
		
		if (FlagsFastCount & _BV(13)) // ~ 2,67 s
		{
			if(TRANSMIT==0x00) // передача разрешена
			{
				TRANSMIT=0x01; // запрет передачи
				usart_data_in_num=67; // кол-во отправляемых байт
				cooler_hand_power=cooler_data[0]/2.55; // преобразование значения
				// START
				buffer[0]=0x3B; 
				buffer[1]=0xa1; // команда - данные состояния
				// time
				buffer[2]=string_1[0];
				buffer[3]=string_1[1];
				buffer[4]=string_1[3];
				buffer[5]=string_1[4];
				buffer[6]=string_1[6];
				buffer[7]=string_1[7];
				// T1
				buffer[8]=string_2[5];
				buffer[9]=string_2[6];
				buffer[10]=string_2[8];
				// T2
				buffer[11]=string_3[5];
				buffer[12]=string_3[6];
				buffer[13]=string_3[8];
				// T3
				buffer[14]=string_4[5];
				buffer[15]=string_4[6];
				buffer[16]=string_4[8];
				// PH
				buffer[17]=string_2[15];
				buffer[18]=string_2[16];
				buffer[19]=string_2[18];
				buffer[20]=string_2[19];
				// Redox
				buffer[21]=string_3[15];
				buffer[22]=string_3[17];
				buffer[23]=string_3[18];
				buffer[24]=string_3[19];
				// status
				buffer[25]=rele_ports_status;
				buffer[26]=rele_hand_status;
				buffer[27]=cooler_chanel_status;
				// k1
				buffer[28]=k1/1000+0x30;
				buffer[29]=(k1%1000)/100+0x30;
				buffer[30]=((k1%1000)%100)/10+0x30;
				buffer[31]=((k1%1000)%100)%10+0x30;
				// k2
				buffer[32]=(k2/1000)/100+0x30;
				buffer[33]=((k2/1000)%100)/10+0x30;
				buffer[34]=((k2/1000)%100)%10+0x30;
				// k
				buffer[35]=k/1000+0x30;
				buffer[36]=(k%1000)/100+0x30;
				buffer[37]=((k%1000)%100)/10+0x30;
				buffer[38]=((k%1000)%100)%10+0x30;
				// состояние режимов
				buffer[39]=mode_status;
				// doliv_porog1
				buffer[40]=doliv_porog1/1000+0x30;
				buffer[41]=(doliv_porog1%1000)/100+0x30;
				buffer[42]=((doliv_porog1%1000)%100)/10+0x30;
				buffer[43]=((doliv_porog1%1000)%100)%10+0x30;
				// doliv_porog2
				buffer[44]=doliv_porog2/1000+0x30;
				buffer[45]=(doliv_porog2%1000)/100+0x30;
				buffer[46]=((doliv_porog2%1000)%100)/10+0x30;
				buffer[47]=((doliv_porog2%1000)%100)%10+0x30;
				// water_lvl
				buffer[48]=water_lvl/1000+0x30;
				buffer[49]=(water_lvl%1000)/100+0x30;
				buffer[50]=((water_lvl%1000)%100)/10+0x30;
				buffer[51]=((water_lvl%1000)%100)%10+0x30;
				buffer[52]=water_lvl_status;
				// LED time
				buffer[53]=lcd_led_stop/100000+0x30;
				buffer[54]=(lcd_led_stop%100000)/10000+0x30;
				buffer[55]=lcd_led_start/100000+0x30;
				buffer[56]=(lcd_led_start%100000)/10000+0x30;
				// COOLER time
				buffer[57]=cooler_stop/100000+0x30;
				buffer[58]=(cooler_stop%100000)/10000+0x30;
				buffer[59]=cooler_start/100000+0x30;
				buffer[60]=(cooler_start%100000)/10000+0x30;
				// COOLER hand power
				buffer[61]=cooler_hand_power/100+0x30;
				buffer[62]=(cooler_hand_power%100)/10+0x30;
				buffer[63]=(cooler_hand_power%100)%10+0x30;
				// CO2 status
				buffer[64]=co2_status;
				// temp. mode status
				buffer[65]=heater_chanel_status;
				// END
				buffer[66]=0xb3;
				
				buffer_start(); // отправка буфера данных
			}
			FlagsFastCount &= ~_BV(13); // сбросить флаг
		};
		
		if (FlagsFastCount & _BV(15)) // ~ 10.6 s
		{
			if(pompa) // помпа включена
			{
				pompa_time_count++; // отсчет времени
				if(pompa_time_count==2) // ~ 21.2 s
				{
					pompa_time_count=0; // обнулить
					if(water_lvl_pompa==water_lvl) WATER_ERROR_POMPA=1; // если предыдущее положение датчика и текущее равны - ошибка
				}
			}
			water_lvl_pompa=water_lvl; // запись предыдущего положения датчика
			FlagsFastCount &= ~_BV(15); // сбросить флаг
		};
	};
	return(0);
}

// функция температурного режима и режима продувки крышки
void set_temp_mode_and_produv(void)
{
	// температурный режим
	switch(temp_control)
	{
		case 1: // установить режим
		{
			switch(set_temp_sensor) // выбор температурного датчика
			{
				case 1: temp_now=(string_2[5]-0x30)*100+(string_2[6]-0x30)*10+(string_2[8]-0x30); break;
				case 2: temp_now=(string_3[5]-0x30)*100+(string_3[6]-0x30)*10+(string_3[8]-0x30); break;
				case 3: temp_now=(string_4[5]-0x30)*100+(string_4[6]-0x30)*10+(string_4[8]-0x30); break;
				default:; break;
			}
			if(temp_now!=0)
			{
				// если текущая температура меньше установленной на значение порога срабатывания
				if((set_temp-temp_now)>set_temp_dec) heater=1; // флаг - включить нагрев
				if(heater) // если нагрев задан
				{
					RELE_PORT_2|=RELE_6; // включить нагреватель
					OCR0=0; // отключить кулера
					if(temp_now>=set_temp) heater=0; // если текущая температура превысила заданную - сбросить флаг 
					heater_chanel_status=0x33; // состояние - температурный режим
				}
				else RELE_PORT_2&=~RELE_6; // отключить нагреватель
				// если текущая температура выше установленной на значение порога срабатывания
				if((temp_now-set_temp)>set_temp_dec) cooler=1; // флаг - включить кулера
				// если задана работа кулеров 
				// и если текущее время находится в промежутке заданного временного интервала работы кулеров
				if(cooler&&((time_now>cooler_start)&&(time_now<cooler_stop)))
				{
					cooler_start_pulse[1]++; // отсчет времени стартового импульса
					// ~3.33 с - устанавливаем заданную мощность
					if(cooler_start_pulse[1]==10) 
					{
						cooler_start_pulse[1]=9; // продолжаем выполнять условие
						cooler_data[1]=cooler_set; // устанавливаем заданную мощность кулеров
						// если мощности кулеров меньше минимальной
						if(cooler_data[1]<cooler_min) cooler_data[1]=cooler_min;
					}
					// мощность кулеров на максимум
					else cooler_data[1]=255;
					OCR0=cooler_data[1];
					// если текущая температура ниже заданной
					if(temp_now<=set_temp) {cooler=0; cooler_start_pulse[1]=0;} // сбросить флаг, сбросить значение стартового импульса
					cooler_chanel_status=0x33; // состояние кулеров - температурный режим
				}
				// отключить кулера
				else {OCR0=0;cooler_start_pulse[1]=0;}		
			}
		} break;
		
		case 2: // сбросить режим
		{
			if(rele_set[5]==1) heater_chanel_status=0x32; 
			// если ручной режим и режим таймеров не активны
			if((rele_set[5]==0)&&(heater_timer_status==0)) RELE_PORT_2&=~RELE_6; // отключить нагреватель	
			// установка значения PWM относительно приоритета режимов
			if(cooler_start_pulse[0]) cooler_start_pulse[0]=0; /*OCR0=cooler_data[0];*/ // 1-ручной режим управления
			if(cooler_start_pulse[3]) OCR0=cooler_data[3]; // 2-режим продувки крышки
			if(cooler_start_pulse[2]) cooler_start_pulse[2]=0; // 3-режим таймеров
			if(cooler_start_pulse[0]!=0) cooler_chanel_status=0x32;
			// если ни один из режимов не запущен - отключить PWM
			if((cooler_start_pulse[0]==0)&(cooler_start_pulse[1]==0)&(cooler_start_pulse[2]==0)&(cooler_start_pulse[3]==0)) OCR0=0;
			temp_control=0;
		}break;
		default:; break;
	}
	
	// режим продувки крышки
	// включение кулеров на заданный интервал времени происходит в начале часа
	switch(produv)
	{
		case 1: // установить режим
		{
			// если текущее время находится в промежутке заданного временного интервала работы кулеров
			if((time_now>=cooler_start)&&(time_now<cooler_stop))
			{
				if(cooler==0) // если кулера не активны
				{
					// начало следующего часа - отсчет периода
					if(time_min_and_sec==0) timer_produv_period_count++;
					// отсчитываемое время продувки крышки находиться в заданном интервале времени
					if((time_min_and_sec>=0)&&(time_min_and_sec<timer_produv_time))
					{
						// отсчитываемый период времени включения продувки совпадает с заданным
						if(timer_produv_period_count==timer_produv_period*3) // *3 - связано с временем опроса (~0.333 с)
						{
							cooler_start_pulse[3]++; // отсчет времени стартового импульса
							// ~3.33 с - устанавливаем заданную мощность
							if(cooler_start_pulse[3]==10) 
							{
								cooler_start_pulse[3]=9; // продолжаем выполнять условие
								// если мощности кулеров меньше минимальной
								cooler_data[3]=cooler_min;
							}
							// мощность кулеров на максимум
							else cooler_data[3]=255;
							OCR0=cooler_data[3];
							cooler_chanel_status=0x34; // состояние кулеров - режим продувки крышки
						}
					}
					else // отключить кулера
					{
						// если режим продувки задан - обнулить переменные
						if(cooler_start_pulse[3]) {cooler_start_pulse[3]=0; timer_produv_period_count=0;}
						// установка значения PWM относительно приоритета режимов
						if(cooler_start_pulse[0]) OCR0=cooler_data[0]; // 1-ручной режим управления
						if(cooler_start_pulse[2]) OCR0=cooler_data[2]; // 2-режим таймеров
						if(cooler_start_pulse[1]) OCR0=cooler_data[1]; // 3-температурный режим
						// если ни один из режимов не запущен - отключить PWM
						if((cooler_start_pulse[0]==0)&(cooler_start_pulse[1]==0)&(cooler_start_pulse[2]==0)&(cooler_start_pulse[3]==0)) OCR0=0;
					}
				}
			}
			else {OCR0=0;cooler_start_pulse[3]=0;} // отключить кулера, сбросить значение стартового импульса
		}break;
		
		case 2: // сбросить режим
		{
			timer_produv_period_count=0; // сбросить значение периода включения
			// установка значения PWM относительно приоритета режимов
			if(cooler_start_pulse[0]) OCR0=cooler_data[0]; // 1-ручной режим управления
			if(cooler_start_pulse[2]) OCR0=cooler_data[2]; // 2-режим таймеров
			if(cooler_start_pulse[1]) OCR0=cooler_data[1]; // 3-температурный режим
			// если ни один из режимов не запущен - отключить PWM
			if((cooler_start_pulse[0]==0)&(cooler_start_pulse[1]==0)&(cooler_start_pulse[2]==0)&(cooler_start_pulse[3]==0)) OCR0=0;
			produv=0; // флаг - режим продувки отключен
		}break;
		default:; break;
	}	
}

// функция записи настроек в EEPROM
// ввиду длительной записи данных в EEPROM функция разделена на этапы
void save_option_eeprom(void)
{
	unsigned char count_i=0;
	eeprom_count++; // отсчет транзакций
	switch(eeprom_count)
	{
		case 1:
		{
			// записать значение - "запись произведена"
			eeprom_write_word((unsigned int*)1000,0x1d);
			// настройки, текущие переменные режимов, коэффициенты
			eeprom_write_dword((unsigned long*)10,k1);
			eeprom_write_dword((unsigned long*)14,k2);
			eeprom_write_dword((unsigned long*)18,k);
			eeprom_write_byte((unsigned char*)22,rele_set[0]);
			eeprom_write_byte((unsigned char*)23,rele_set[1]);
			eeprom_write_byte((unsigned char*)24,rele_set[2]);
			eeprom_write_byte((unsigned char*)25,rele_set[3]);
			eeprom_write_byte((unsigned char*)26,rele_set[4]);
			eeprom_write_byte((unsigned char*)27,rele_set[5]);
		}break;
		case 2:
		{
			eeprom_write_byte((unsigned char*)28,rele_set[6]);
			eeprom_write_byte((unsigned char*)29,rele_set[7]);
			eeprom_write_byte((unsigned char*)30,cooler_start_pulse[0]);
			eeprom_write_byte((unsigned char*)31,cooler_start_pulse[1]);
			eeprom_write_byte((unsigned char*)32,cooler_start_pulse[2]);
			eeprom_write_byte((unsigned char*)33,cooler_start_pulse[3]);
			eeprom_write_byte((unsigned char*)34,cooler_data[0]);
			eeprom_write_byte((unsigned char*)35,cooler_data[1]);
			eeprom_write_byte((unsigned char*)36,cooler_data[2]);
			eeprom_write_byte((unsigned char*)37,cooler_data[3]);
			eeprom_write_byte((unsigned char*)38,cooler_min);
			eeprom_write_byte((unsigned char*)39,cooler_chanel_status);
			eeprom_write_byte((unsigned char*)40,OCR0);
			eeprom_write_dword((unsigned long*)41,lcd_led_stop);
			eeprom_write_dword((unsigned long*)45,lcd_led_start);
		}break;
		case 3:
		{
			eeprom_write_dword((unsigned long*)49,cooler_stop);
			eeprom_write_dword((unsigned long*)53,cooler_start);
			eeprom_write_byte((unsigned char*)57,temp_control);
			eeprom_write_byte((unsigned char*)58,set_temp_sensor);
			eeprom_write_word((unsigned int*)59,set_temp);
			eeprom_write_byte((unsigned char*)61,set_temp_dec);
			eeprom_write_word((unsigned int*)62,cooler_set);
			eeprom_write_byte((unsigned char*)64,cooler);
			eeprom_write_byte((unsigned char*)65,heater);
		}break;
		case 4:
		{
			count_i=0;
			eeprom_write_dword((unsigned long*)(66+(count_i*4)), (unsigned long)timer1_start[count_i]);
			eeprom_write_dword((unsigned long*)(102+(count_i*4)),(unsigned long)timer2_start[count_i]);
			eeprom_write_dword((unsigned long*)(138+(count_i*4)),(unsigned long)timer3_start[count_i]);
			eeprom_write_dword((unsigned long*)(174+(count_i*4)),(unsigned long)timer1_stop[count_i]);
			eeprom_write_dword((unsigned long*)(210+(count_i*4)),(unsigned long)timer2_stop[count_i]);
			eeprom_write_dword((unsigned long*)(246+(count_i*4)),(unsigned long)timer3_stop[count_i]);
		}break;
		case 5:
		{
			count_i=1;
			eeprom_write_dword((unsigned long*)(66+(count_i*4)), (unsigned long)timer1_start[count_i]);
			eeprom_write_dword((unsigned long*)(102+(count_i*4)),(unsigned long)timer2_start[count_i]);
			eeprom_write_dword((unsigned long*)(138+(count_i*4)),(unsigned long)timer3_start[count_i]);
			eeprom_write_dword((unsigned long*)(174+(count_i*4)),(unsigned long)timer1_stop[count_i]);
			eeprom_write_dword((unsigned long*)(210+(count_i*4)),(unsigned long)timer2_stop[count_i]);
			eeprom_write_dword((unsigned long*)(246+(count_i*4)),(unsigned long)timer3_stop[count_i]);
		}break;
		case 6:
		{
			count_i=2;
			eeprom_write_dword((unsigned long*)(66+(count_i*4)), (unsigned long)timer1_start[count_i]);
			eeprom_write_dword((unsigned long*)(102+(count_i*4)),(unsigned long)timer2_start[count_i]);
			eeprom_write_dword((unsigned long*)(138+(count_i*4)),(unsigned long)timer3_start[count_i]);
			eeprom_write_dword((unsigned long*)(174+(count_i*4)),(unsigned long)timer1_stop[count_i]);
			eeprom_write_dword((unsigned long*)(210+(count_i*4)),(unsigned long)timer2_stop[count_i]);
			eeprom_write_dword((unsigned long*)(246+(count_i*4)),(unsigned long)timer3_stop[count_i]);
		}break;
		case 7:
		{
			count_i=3;
			eeprom_write_dword((unsigned long*)(66+(count_i*4)), (unsigned long)timer1_start[count_i]);
			eeprom_write_dword((unsigned long*)(102+(count_i*4)),(unsigned long)timer2_start[count_i]);
			eeprom_write_dword((unsigned long*)(138+(count_i*4)),(unsigned long)timer3_start[count_i]);
			eeprom_write_dword((unsigned long*)(174+(count_i*4)),(unsigned long)timer1_stop[count_i]);
			eeprom_write_dword((unsigned long*)(210+(count_i*4)),(unsigned long)timer2_stop[count_i]);
			eeprom_write_dword((unsigned long*)(246+(count_i*4)),(unsigned long)timer3_stop[count_i]);
		}break;
		case 8:
		{
			count_i=4;
			eeprom_write_dword((unsigned long*)(66+(count_i*4)), (unsigned long)timer1_start[count_i]);
			eeprom_write_dword((unsigned long*)(102+(count_i*4)),(unsigned long)timer2_start[count_i]);
			eeprom_write_dword((unsigned long*)(138+(count_i*4)),(unsigned long)timer3_start[count_i]);
			eeprom_write_dword((unsigned long*)(174+(count_i*4)),(unsigned long)timer1_stop[count_i]);
			eeprom_write_dword((unsigned long*)(210+(count_i*4)),(unsigned long)timer2_stop[count_i]);
			eeprom_write_dword((unsigned long*)(246+(count_i*4)),(unsigned long)timer3_stop[count_i]);
		}break;
		case 9:
		{
			count_i=5;
			eeprom_write_dword((unsigned long*)(66+(count_i*4)), (unsigned long)timer1_start[count_i]);
			eeprom_write_dword((unsigned long*)(102+(count_i*4)),(unsigned long)timer2_start[count_i]);
			eeprom_write_dword((unsigned long*)(138+(count_i*4)),(unsigned long)timer3_start[count_i]);
			eeprom_write_dword((unsigned long*)(174+(count_i*4)),(unsigned long)timer1_stop[count_i]);
			eeprom_write_dword((unsigned long*)(210+(count_i*4)),(unsigned long)timer2_stop[count_i]);
			eeprom_write_dword((unsigned long*)(246+(count_i*4)),(unsigned long)timer3_stop[count_i]);
		}break;
		case 10:
		{
			count_i=6;
			eeprom_write_dword((unsigned long*)(66+(count_i*4)), (unsigned long)timer1_start[count_i]);
			eeprom_write_dword((unsigned long*)(102+(count_i*4)),(unsigned long)timer2_start[count_i]);
			eeprom_write_dword((unsigned long*)(138+(count_i*4)),(unsigned long)timer3_start[count_i]);
			eeprom_write_dword((unsigned long*)(174+(count_i*4)),(unsigned long)timer1_stop[count_i]);
			eeprom_write_dword((unsigned long*)(210+(count_i*4)),(unsigned long)timer2_stop[count_i]);
			eeprom_write_dword((unsigned long*)(246+(count_i*4)),(unsigned long)timer3_stop[count_i]);
		}break;
		case 11:
		{
			count_i=7;
			eeprom_write_dword((unsigned long*)(66+(count_i*4)), (unsigned long)timer1_start[count_i]);
			eeprom_write_dword((unsigned long*)(102+(count_i*4)),(unsigned long)timer2_start[count_i]);
			eeprom_write_dword((unsigned long*)(138+(count_i*4)),(unsigned long)timer3_start[count_i]);
			eeprom_write_dword((unsigned long*)(174+(count_i*4)),(unsigned long)timer1_stop[count_i]);
			eeprom_write_dword((unsigned long*)(210+(count_i*4)),(unsigned long)timer2_stop[count_i]);
			eeprom_write_dword((unsigned long*)(246+(count_i*4)),(unsigned long)timer3_stop[count_i]);
		}break;
		case 12:
		{
			count_i=8;
			eeprom_write_dword((unsigned long*)(66+(count_i*4)), (unsigned long)timer1_start[count_i]);
			eeprom_write_dword((unsigned long*)(102+(count_i*4)),(unsigned long)timer2_start[count_i]);
			eeprom_write_dword((unsigned long*)(138+(count_i*4)),(unsigned long)timer3_start[count_i]);
			eeprom_write_dword((unsigned long*)(174+(count_i*4)),(unsigned long)timer1_stop[count_i]);
			eeprom_write_dword((unsigned long*)(210+(count_i*4)),(unsigned long)timer2_stop[count_i]);
			eeprom_write_dword((unsigned long*)(246+(count_i*4)),(unsigned long)timer3_stop[count_i]);
		}break;
		case 13:
		{
			eeprom_write_byte((unsigned char*)282,produv);
			eeprom_write_dword((unsigned long*)283,timer_produv_time);
			eeprom_write_byte((unsigned char*)287,timer_produv_period);
			eeprom_write_byte((unsigned char*)288,co2);
			eeprom_write_word((unsigned int*)289,ph_high);
			eeprom_write_word((unsigned int*)291,ph_low);
			eeprom_write_dword((unsigned long*)293,co2_stop);
			eeprom_write_dword((unsigned long*)297,co2_start);
			eeprom_write_byte((unsigned char*)301,heater_timer_status);
		}
		case 14:
		{
			eeprom_write_byte((unsigned char*)302,co2_timer_status);
			eeprom_write_byte((unsigned char*)303,co2_set);
			eeprom_write_word((unsigned int*)304,water_lvl);
			eeprom_write_word((unsigned int*)306,doliv_porog1);
			eeprom_write_word((unsigned int*)308,doliv_porog2);
			eeprom_write_byte((unsigned char*)310,water_lvl_status);
			eeprom_write_byte((unsigned char*)311,co2_status);
			eeprom_write_byte((unsigned char*)312,doliv);
			eeprom_write_byte((unsigned char*)313,heater_chanel_status);
			EEPROM_WRITE=0;
		}
		default:;break;
	}
}

// функция чтения настроек из EEPROM 
void load_option_eeprom(void) // функция чтения из EEPROM
{
	k1=eeprom_read_dword((unsigned long*)10);
	k2=eeprom_read_dword((unsigned long*)14);
	k=eeprom_read_dword((unsigned long*)18);
	rele_set[0]=eeprom_read_byte((unsigned char*)22);
	rele_set[1]=eeprom_read_byte((unsigned char*)23);
	rele_set[2]=eeprom_read_byte((unsigned char*)24);
	rele_set[3]=eeprom_read_byte((unsigned char*)25);
	rele_set[4]=eeprom_read_byte((unsigned char*)26);
	rele_set[5]=eeprom_read_byte((unsigned char*)27);
	rele_set[6]=eeprom_read_byte((unsigned char*)28);
	rele_set[7]=eeprom_read_byte((unsigned char*)29); 
	cooler_start_pulse[0]=eeprom_read_byte((unsigned char*)30);
	cooler_start_pulse[1]=eeprom_read_byte((unsigned char*)31);
	cooler_start_pulse[2]=eeprom_read_byte((unsigned char*)32);
	cooler_start_pulse[3]=eeprom_read_byte((unsigned char*)33);
	cooler_data[0]=eeprom_read_byte((unsigned char*)34);
	cooler_data[1]=eeprom_read_byte((unsigned char*)35);
	cooler_data[2]=eeprom_read_byte((unsigned char*)36);
	cooler_data[3]=eeprom_read_byte((unsigned char*)37);
	cooler_min=eeprom_read_byte((unsigned char*)38);
	cooler_chanel_status=eeprom_read_byte((unsigned char*)39);
	OCR0=eeprom_read_byte((unsigned char*)40);
	lcd_led_stop=eeprom_read_dword((unsigned long*)41);
	lcd_led_start=eeprom_read_dword((unsigned long*)45);
	cooler_stop=eeprom_read_dword((unsigned long*)49);
	cooler_start=eeprom_read_dword((unsigned long*)53);
	temp_control=eeprom_read_byte((unsigned char*)57);
	set_temp_sensor=eeprom_read_byte((unsigned char*)58);
	set_temp=eeprom_read_word((unsigned int*)59);
	set_temp_dec=eeprom_read_byte((unsigned char*)61);
	cooler_set=eeprom_read_word((unsigned int*)62);
	cooler=eeprom_read_byte((unsigned char*)64);
	heater=eeprom_read_byte((unsigned char*)65);
	for(unsigned char i=0;i<9;i++)
	{
		timer1_start[i]=eeprom_read_dword((unsigned long*)(66+(i*4)));
		timer2_start[i]=eeprom_read_dword((unsigned long*)(102+(i*4)));
		timer3_start[i]=eeprom_read_dword((unsigned long*)(138+(i*4)));
		timer1_stop[i]=eeprom_read_dword((unsigned long*)(174+(i*4)));
		timer2_stop[i]=eeprom_read_dword((unsigned long*)(210+(i*4)));
		timer3_stop[i]=eeprom_read_dword((unsigned long*)(246+(i*4)));
	}
	produv=eeprom_read_byte((unsigned char*)282);
	timer_produv_time=eeprom_read_dword((unsigned long*)283);
	timer_produv_period=eeprom_read_byte((unsigned char*)287);
	co2=eeprom_read_byte((unsigned char*)288);
	ph_high=eeprom_read_word((unsigned int*)289);
	ph_low=eeprom_read_word((unsigned int*)291);
	co2_stop=eeprom_read_dword((unsigned long*)293);
	co2_start=eeprom_read_dword((unsigned long*)297);
	heater_timer_status=eeprom_read_byte((unsigned char*)301);
	co2_timer_status=eeprom_read_byte((unsigned char*)302);
	co2_set=eeprom_read_byte((unsigned char*)303);
	water_lvl=eeprom_read_word((unsigned int*)304);
	doliv_porog1=eeprom_read_word((unsigned int*)306);
	doliv_porog2=eeprom_read_word((unsigned int*)308);
	water_lvl_status=eeprom_read_byte((unsigned char*)310);
	co2_status=eeprom_read_byte((unsigned char*)311);
	doliv=eeprom_read_byte((unsigned char*)312);
	heater_chanel_status=eeprom_read_byte((unsigned char*)313);
}

// функция отправки данных таймеров на ПК
void timers_data_to_uart(void)
{
	if(TRANSMIT==0x00) // передача разрешена
	{
		TRANSMIT=0x01; // запрет передачи
		usart_data_in_num=327; // кол-во отправляемых байт
		
		buffer[0]=0x3B; // начало пакета
		buffer[1]=0xa2; // команда - данные таймеров
		// данные таймеров
		for(unsigned char i=0;i<9;i++)
		{
			buffer[2+36*i]=timer1_start[i]/100000+0x30;
			buffer[3+36*i]=(timer1_start[i]%100000)/10000+0x30;
			buffer[4+36*i]=((timer1_start[i]%100000)%10000)/1000+0x30;
			buffer[5+36*i]=(((timer1_start[i]%100000)%10000)%1000)/100+0x30;
			buffer[6+36*i]=((((timer1_start[i]%100000)%10000)%1000)%100)/10+0x30;
			buffer[7+36*i]=((((timer1_start[i]%100000)%10000)%1000)%100)%10+0x30;
			
			buffer[8+36*i]=timer2_start[i]/100000+0x30;
			buffer[9+36*i]=(timer2_start[i]%100000)/10000+0x30;
			buffer[10+36*i]=((timer2_start[i]%100000)%10000)/1000+0x30;
			buffer[11+36*i]=(((timer2_start[i]%100000)%10000)%1000)/100+0x30;
			buffer[12+36*i]=((((timer2_start[i]%100000)%10000)%1000)%100)/10+0x30;
			buffer[13+36*i]=((((timer2_start[i]%100000)%10000)%1000)%100)%10+0x30;
			
			buffer[14+36*i]=timer3_start[i]/100000+0x30;
			buffer[15+36*i]=(timer3_start[i]%100000)/10000+0x30;
			buffer[16+36*i]=((timer3_start[i]%100000)%10000)/1000+0x30;
			buffer[17+36*i]=(((timer3_start[i]%100000)%10000)%1000)/100+0x30;
			buffer[18+36*i]=((((timer3_start[i]%100000)%10000)%1000)%100)/10+0x30;
			buffer[19+36*i]=((((timer3_start[i]%100000)%10000)%1000)%100)%10+0x30;
			
			buffer[20+36*i]=timer1_stop[i]/100000+0x30;
			buffer[21+36*i]=(timer1_stop[i]%100000)/10000+0x30;
			buffer[22+36*i]=((timer1_stop[i]%100000)%10000)/1000+0x30;
			buffer[23+36*i]=(((timer1_stop[i]%100000)%10000)%1000)/100+0x30;
			buffer[24+36*i]=((((timer1_stop[i]%100000)%10000)%1000)%100)/10+0x30;
			buffer[25+36*i]=((((timer1_stop[i]%100000)%10000)%1000)%100)%10+0x30;
			
			buffer[26+36*i]=timer2_stop[i]/100000+0x30;
			buffer[27+36*i]=(timer2_stop[i]%100000)/10000+0x30;
			buffer[28+36*i]=((timer2_stop[i]%100000)%10000)/1000+0x30;
			buffer[29+36*i]=(((timer2_stop[i]%100000)%10000)%1000)/100+0x30;
			buffer[30+36*i]=((((timer2_stop[i]%100000)%10000)%1000)%100)/10+0x30;
			buffer[31+36*i]=((((timer2_stop[i]%100000)%10000)%1000)%100)%10+0x30;
			
			buffer[32+36*i]=timer3_stop[i]/100000+0x30;
			buffer[33+36*i]=(timer3_stop[i]%100000)/10000+0x30;
			buffer[34+36*i]=((timer3_stop[i]%100000)%10000)/1000+0x30;
			buffer[35+36*i]=(((timer3_stop[i]%100000)%10000)%1000)/100+0x30;
			buffer[36+36*i]=((((timer3_stop[i]%100000)%10000)%1000)%100)/10+0x30;
			buffer[37+36*i]=((((timer3_stop[i]%100000)%10000)%1000)%100)%10+0x30;
		}
		buffer[326]=0xb3; // конец пакета
		
		buffer_start(); // отправка данных
	}
}

// функция отправки данных режимов на ПК
void events_data_to_uart(void)
{	
	TRANSMIT=0x01; // запрет передачи
	usart_data_in_num=28; // кол-во отправляемых байт
	
	buffer[0]=0x3B; // начало пакета
	buffer[1]=0xa3; // команда - данные режимов
	// данные режимов
	// установленный датчик температурного режима
	buffer[2]=set_temp_sensor+0x30;
	// установленная температура
	buffer[3]=set_temp/100+0x30;
	buffer[4]=(set_temp%100)/10+0x30;
	// установленный порог срабатывания
	buffer[5]=set_temp_dec+0x30;
	// мощность кулеров температурного режима
	buffer[6]=cooler_set/100+0x30;
	buffer[7]=(cooler_set%100)/10+0x30;
	buffer[8]=(cooler_set%100)%10+0x30;
	// время работы кулеров в режиме продувки 
	buffer[9]=timer_produv_time/1000+0x30;
	buffer[10]=(timer_produv_time%1000)/100+0x30;
	buffer[11]=((timer_produv_time%1000)%100)/10+0x30;
	buffer[12]=((timer_produv_time%1000)%100)%10+0x30;
	// период включения кулеров в режиме продувки
	buffer[13]=timer_produv_period+0x30;
	// минимальная мощность кулеров
	buffer[14]=cooler_min/100+0x30;
	buffer[15]=((unsigned int)cooler_min%100)/10+0x30;
	buffer[16]=((unsigned int)cooler_min%100)%10+0x30;
	// значение РН - включение СО2
	buffer[17]=ph_high/100+0x30;
	buffer[18]=(ph_high%100)/10+0x30;
	buffer[19]=(ph_high%100)%10+0x30;
	// значение РН - отключение СО2
	buffer[20]=ph_low/100+0x30;
	buffer[21]=(ph_low%100)/10+0x30;
	buffer[22]=(ph_low%100)%10+0x30;
	// временные интервалы работы СО2 - отключить
	buffer[23]=(co2_stop/10000)/10+0x30;
	buffer[24]=(co2_stop/10000)%10+0x30;
	// временные интервалы работы СО2 - включить
	buffer[25]=(co2_start/10000)/10+0x30;
	buffer[26]=(co2_start/10000)%10+0x30;
	
	buffer[27]=0xb3; // конец пакета
		
	buffer_start(); // отправка данных
}

// функция автоматического долива воды
void water_doliv(void)
{
	switch(doliv) 
	{
		case 1: // установить режим
		{
			// если произошел резкий скачек уровня воды - отсчитываем 10 с
			// если за это время показания датчика не восстановятся в заданных пределах - датчик не исправен
			// режим отключается и отображается состояние ошибки
			if((abs(water_lvl-water_lvl_last)>=abs((doliv_porog2-doliv_porog1)/2))|(WATER_ERROR_POMPA)) 
			{
				WATER_ERROR=1; // флаг - датчик не исправен
				water_lvl_status=0x34; // состояние режима 
				water_lvl_ovf_time++; // отсчет времени
			}
			else 
			{
				// если за ~10 с значения датчика восстановились
				if(water_lvl_ovf_time<30) 
				{
					WATER_ERROR=0; // флаг - датчик исправен
					water_lvl_ovf_time=0; // сбросить отсчет вермени
				}
			}
			// если датчик исправен
			if(!WATER_ERROR)
			{
				// уровень воды достиг нижнего порога (2)
				if(water_lvl<=doliv_porog2) pompa=1; // флаг - включить помпу
				// уровень воды достиг верхнего порога (1)
				if(water_lvl>=doliv_porog1) pompa=0; // флаг - отключить помпу
				// работа помпы задана
				if(pompa)
				{
					RELE_PORT_2|=RELE_8; // включить помпу
					water_lvl_status=0x33; // состояние - режим автоматического долива
				}
				else RELE_PORT_2&=~RELE_8; // выключить помпу
				// записать текущее значение датчика для последующего сравнения уровня воды
				water_lvl_last=water_lvl; 
			}
		}break;
		
		case 2: // сбросить режим
		{
			if(rele_set[7]==1) water_lvl_status=0x32;
			// если не задан ручной режим и режим таймеров - отключить помпу
			if((rele_set[7]==0)&&(doliv_timer_status==0)) {RELE_PORT_2&=~RELE_8; water_lvl_status=0x30;}
			// сбросить флаги
			doliv=0; 
			water_lvl_ovf_time=0;
			WATER_ERROR=0;
		}break;
		default:; break;
	}
}