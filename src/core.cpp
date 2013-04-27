extern "C" {
#include "stm32f0xx_conf.h"
void delay_us(uint32_t us);
}

#define LCD_GPIO GPIOA
#define LCD_DB3 (GPIO_Pin_0)
#define LCD_DB2 (GPIO_Pin_1)
#define LCD_DB1 (GPIO_Pin_2)
#define LCD_DB0 (GPIO_Pin_3)
#define LCD_EN (GPIO_Pin_5)
#define LCD_RS (GPIO_Pin_6)

class LCDDisplay
{
public:
	enum Type {
		CommandType,
		CharactersType
	};

	LCDDisplay()
	{
	}

	void initialize()
	{
		GPIO_Write(LCD_GPIO, LCD_EN);
		delay_us(15000);
		displayWrite(CommandType, 0x3);
		delay_us(4100);
		displayWrite(CommandType, 0x3);
		delay_us(100);
		displayWrite(CommandType, 0x3);
		delay_us(40);
		displayWrite(CommandType, 0x2);
		delay_us(40);
		displayWrite(CommandType, 0x28);
		displayWrite(CommandType, 0x0c);
		displayWrite(CommandType, 0x06);
		displayWrite(CommandType, 0x01);
		delay_us(1640);


		displayWrite(CharactersType, 'H');
		displayWrite(CharactersType, 'e');
		displayWrite(CharactersType, 'l');
		displayWrite(CharactersType, 'l');
		displayWrite(CharactersType, 'o');
		displayWrite(CharactersType, ' ');
		displayWrite(CharactersType, 'W');
		displayWrite(CharactersType, 'o');
		displayWrite(CharactersType, 'r');
		displayWrite(CharactersType, 'l');
		displayWrite(CharactersType, 'd');
	}

	void displayWrite(Type type, unsigned char data)
	{
		if (type == CharactersType) {
			GPIO_Write(LCD_GPIO, LCD_EN | LCD_RS);
		}
		else {
			GPIO_Write(LCD_GPIO, LCD_EN);
		}

		dataPartWrite(data >> 4);
		delay_us(40);
		GPIO_ResetBits(LCD_GPIO, LCD_EN);
		delay_us(40);
		GPIO_SetBits(LCD_GPIO, LCD_EN);
		dataPartWrite(data & 0x0f);
		delay_us(40);
		GPIO_ResetBits(LCD_GPIO, LCD_EN);
		delay_us(40);
		GPIO_SetBits(LCD_GPIO, LCD_EN);

	}

private:
	inline void dataPartWrite(unsigned char dataPart)
	{
		//LCD_GPIO->ODR = (LCD_GPIO->ODR & ~(0x0f)) | (((dataPart >> 3) & 0x01) << 0) | (((dataPart >> 2) & 0x01) << 1) | (((dataPart >> 1) & 0x01) << 2) | (((dataPart >> 0) & 0x01) << 3);
		LCD_GPIO->ODR = (LCD_GPIO->ODR & ~(LCD_DB0 | LCD_DB1 | LCD_DB2 | LCD_DB3)) |
			(dataPart & 0x08 ? LCD_DB3 : 0) |
			(dataPart & 0x04 ? LCD_DB2 : 0) |
			(dataPart & 0x02 ? LCD_DB1 : 0) |
			(dataPart & 0x01 ? LCD_DB0 : 0);
	}
};


extern "C" {

void SysTick_Handler(void)
{
	GPIOC->ODR ^= (1 << 8);
	SysTick->CTRL = 0x00;
	SysTick->VAL = 0X00;
}

#define US_TICKS 48
void delay_us(uint32_t us)
{
	SysTick->LOAD  = ((US_TICKS * us) & SysTick_LOAD_RELOAD_Msk) - 1;
	NVIC_SetPriority (SysTick_IRQn, (1 << __NVIC_PRIO_BITS) - 1);
	SysTick->VAL = 0;
	SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk;

	PWR_EnterSleepMode(PWR_SLEEPEntry_WFI);
}

void mainprog(void)
{
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_5 | GPIO_Pin_6;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	LCDDisplay display;
	display.initialize();

	while (1) {
		delay_us(40000);
		GPIOC->ODR ^= (1 << 9);
	}
}

}
