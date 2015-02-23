/*	АКВАКОНТРОЛЛЕР 2014 В. 1.0.0.
	
	Библиотека для работы с шиной данных I2C.
	Инициализация и работа с часами реального времени DS1307N
	
	Пустоводов С.А. 2014г.
*/

#ifndef I2C_H
#define I2C_H
#endif
// определители
#define SETSDA() I2C_DDR&=~SDA	 	// установить SDA
#define SETSCL() I2C_DDR&=~SCL		// установить SCL
#define RSTSDA() I2C_DDR|= SDA		// сбросить SDA
#define RSTSCL() I2C_DDR|= SCL	    // сбросить SCL
// глобальные переменные
unsigned char data_i2c_h=0;
unsigned char data_i2c_l=0;

void i2c_init(void) // функция инициализации
{
	// настройка выводов
	SETSDA(); 
	SETSCL();
	I2C_PORT&=~SDA;
	I2C_PORT&=~SCL;
}

void i2c_busy(void) // функция, проверяющая занятость шины
{
	while(I2C_DDR&(SDA|SCL)); // ожидание 
}

void i2c_start(void) // функция формирования сигнала начала передачи данных
{
	SETSDA();		// установка сигнала SDA
	SETSCL();		// установка сигнала SCL
	delay_us(2);	// задержка
	RSTSDA();		// сброс сигнала SDA
	delay_us(2);	// задержка
	RSTSCL();		// сброс сигнала SCL
	delay_us(2);	// задержка
}

void i2c_stop(void) // функция формирования сигнала завершения обмена данными
{
	RSTSDA();		// сброс сигнала SDA
	SETSCL();		// установка сигнала SCL
	delay_us(2); 	// задержка
	SETSDA();		// установка сигнала SDA
	delay_us(2); 	// задержка
}

unsigned char i2c_sendbyte(char data) // функция посылки 1 байта; параметр – передаваемый байт
{
	unsigned char i=8;	// счетчик бит
	unsigned char acknowledge=0; // подтверждение
	do 
	{	
		// в цикле выполняется посылка 8 бит
		if((data&0x80)!=0) // если старший бит 1
		{
			SETSDA();	// установка сигнала SDA если старший бит равен 1
		}
		else RSTSDA();	// иначе сброс сигнала SDA
		data=data << 1;	// сдвиг передаваемого байта на 1 разряд влево
		delay_us(2);	// формирование задержки
		// формирование тактового импульса
		SETSCL(); 		// установка сигнала SCL
		delay_us(2);	// задержка
		RSTSCL();		// сброс сигнала SCL
		delay_us(2);	// задержка 
	}
	while (--i);		// цикл выполняется 8 раз
	// формирование тактового импульса для бита подтверждения
	RSTSDA();			// сброс сигала SDA
	SETSCL();			// установка сигнала SCL
	delay_us(2);		// задержка
	// проверка бита подтверждения 
	if((I2C_PIN&SDA)==0) acknowledge=1; // если на линии SDA низкий уровень, бит подтверждения принят
	RSTSCL(); 			// сброс сигнала SCL
	delay_us(2);		// задержка
	return acknowledge; // если бит подтверждения принят, возвр.1
}

unsigned char i2c_readbyte(unsigned char acknowledge) // функция чтения одного байта
{
	unsigned char i=8; // счетчик
	unsigned char i2c_data=0;	// данные шины
	char data; // состояние порта
	I2C_DDR&=~SDA;			// перевод линии SDA в третье состояние
	do 
	{
		i2c_data<<=1;		// сдвиг на 1 разряд влево
		SETSCL(); 			// установка сигнала SCL
		delay_us(2);		// задержка
		data=I2C_PIN;		// считывание состояния порта 
		data=(data&SDA)>>SDA_N;// выделение 0 бита
		i2c_data|=data;		// копирование 0 бита в переменную
		RSTSCL();			// сброс сигнала SCL
		delay_us(2);		// задержка
	}
	while (--i);			// цикл выполняется 8 раз
	if (acknowledge)		// если необходим бит подтверждения
	{
		RSTSDA();			// сброс сигнала SDA
	}
	// формирование тактового импульса
	SETSCL();			// установка сигнала SCL
	delay_us(2); 		// задержка
	RSTSCL();			// сброс сигнала SCL
	return i2c_data;	// функция возвращает принятый байт
}

// ----- функции работы с часами DS1307N ----- //
// функция записи данных
void DS_write(unsigned char addr, unsigned char data)
{
	i2c_start(); // начало передачи
	i2c_sendbyte(0xD0); // команда
	i2c_sendbyte(addr); // адрес
	i2c_sendbyte(data); // данные
	i2c_stop(); // конец передачи
}

// функция чтения данных
unsigned char DS_read(unsigned char addr)
{
	unsigned char temp;
	i2c_start(); // начало передачи 
	i2c_sendbyte(0xD0); // команда
	i2c_sendbyte(addr); // адрес
	i2c_stop(); // конец передачи
	i2c_start(); // начало передачи
	i2c_sendbyte(0xD1); // команда
	temp=i2c_readbyte(0); // данные
	i2c_stop(); // конец передачи
	return temp; // вернуть значение
}

void DS1307_init(void) // инициализация DS1307N
{
	unsigned char i=0;
	i=DS_read(0x02);
	if((i&0x40)!=0)
	{
		DS_write(0x02,i&~0x40);
	}
	i=DS_read(0x00);
	if((i&0x80)!=0)
	{
		DS_write(0x00,i&~0x80);
	}
}

void DS1307(void) // функция опроса
{
	i2c_start();
	i2c_sendbyte(0xD0);
	i2c_sendbyte(0x00);
	i2c_start();
	i2c_sendbyte(0xD1);
	buffer_ds[0]=i2c_readbyte(1);
	buffer_ds[1]=i2c_readbyte(1);
	buffer_ds[2]=i2c_readbyte(0);
	i2c_stop();
}

unsigned char IntToDec(unsigned char data) // преобразование в десятичное число
{
	data=data%100;
	return data/10*16+data%10;
}

void DS1307_set_time(void) // функция опроса
{
	DS_write(0x02,IntToDec(buffer_time[0])); // часы
	DS_write(0x01,IntToDec(buffer_time[1])); // минуты
	DS_write(0x00,IntToDec(buffer_time[2])); // секунды
}
