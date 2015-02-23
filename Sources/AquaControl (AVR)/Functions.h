/* 	АКВАКОНТРОЛЛЕР 2014 В. 1.0.0.
	
	Определения ассемблерных команд
	
	Пустоводов С.А. 2014г.
*/



#define InvBit(p,n) (p^=_BV(n))	//Функция, делает инверсию бита, в перем., регистре, порту ВВ.
#define WDR() __asm__ __volatile__ ("wdr");	// Сброс Сторожевого Таймера
#define NOP() __asm__ __volatile__ ("nop");	// "Пустая" команда
#define SWAP(r) 			asm volatile("swap %0" : "=r" (r) : "0" (r));
#define SBI(port,bit) asm volatile("sbi %0,%1"	:	: "I" (_SFR_IO_ADDR(port)),	"I" (bit));
#define CBI(port,bit) asm volatile("cbi %0,%1"	:	: "I" (_SFR_IO_ADDR(port)),	"I" (bit));

