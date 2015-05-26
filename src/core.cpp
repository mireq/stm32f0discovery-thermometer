#define US_TICKS 48
#define UCC 2940
#define U_ANALOG_MAX 43000.0f

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

// the 8 arrays that form each segment of the custom numbers
unsigned char LT[8] = 
{
	0b00111,
	0b01111,
	0b11111,
	0b11111,
	0b11111,
	0b11111,
	0b11111,
	0b11111
};
unsigned char UB[8] =
{
	0b11111,
	0b11111,
	0b11111,
	0b00000,
	0b00000,
	0b00000,
	0b00000,
	0b00000
};
unsigned char RT[8] =
{
	0b11100,
	0b11110,
	0b11111,
	0b11111,
	0b11111,
	0b11111,
	0b11111,
	0b11111
};
unsigned char LL[8] =
{
	0b11111,
	0b11111,
	0b11111,
	0b11111,
	0b11111,
	0b11111,
	0b01111,
	0b00111
};
unsigned char LB[8] =
{
	0b00000,
	0b00000,
	0b00000,
	0b00000,
	0b00000,
	0b11111,
	0b11111,
	0b11111
};
unsigned char LR[8] =
{
	0b11111,
	0b11111,
	0b11111,
	0b11111,
	0b11111,
	0b11111,
	0b11110,
	0b11100
};
unsigned char MB[8] =
{
	0b11111,
	0b11111,
	0b11111,
	0b00000,
	0b00000,
	0b00000,
	0b11111,
	0b11111
};
unsigned char block[8] =
{
	0b11111,
	0b11111,
	0b11111,
	0b11111,
	0b11111,
	0b11111,
	0b11111,
	0b11111
};

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

	void writeString(const char *string)
	{
		while (*string != '\0') {
			displayWrite(CharactersType, *string);
			string++;
		}
	}

	void writeChar(uint8_t ch)
	{
		displayWrite(CharactersType, ch);
	}

	void writeNumber(uint32_t number)
	{
		static const uint32_t base[] = { 1000000000, 100000000, 10000000, 1000000, 100000, 10000, 1000, 100, 10, 1 };
		char numString[] = "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";
		for (int pos = 0; pos < 10; ++pos) {
			unsigned char digit = number / base[pos];
			number -= digit * base[pos];
			numString[pos] = '0' + digit;
		}
		writeString(numString);
	}

	void writeTemperature(int number)
	{
		static const uint32_t base[] = { 100, 10, 1 };
		for (int pos = 0; pos < 3; ++pos) {
			uint8_t digit = number / base[pos];
			number -= digit * base[pos];
			writeBigNumberAt(digit, pos < 2 ? pos * 4 : (pos * 4 + 2));
		}
		setCursor(8, 1);
		writeChar(4);
		setCursor(14, 0);
		writeChar('o');
	}

	void home()
	{
		displayWrite(CommandType, 0x02);
		delay_us(1640);
	}

	void setCursor(int col, int row)
	{
		displayWrite(CommandType, 0x80 | col | (row == 0 ? 0 : 0x40));
		delay_us(1640);
	}

	void createChar(uint8_t location, uint8_t *charmap)
	{
		displayWrite(CommandType, 0x40 | (location << 3));
		for (int i = 0; i < 8; ++i) {
			displayWrite(CharactersType, charmap[i]);
		}
		delay_us(1640);
	}

	void writeBigNumberAt(uint8_t number, int pos)
	{
		setCursor(pos, 0);
		switch (number) {
			case 0:
				writeChar(0);
				writeChar(1);
				writeChar(2);
				break;
			case 1:
				writeChar(1);
				writeChar(2);
				writeChar(' ');
				break;
			case 2:
				writeChar(6);
				writeChar(6);
				writeChar(2);
				break;
			case 3:
				writeChar(6);
				writeChar(6);
				writeChar(2);
				break;
			case 4:
				writeChar(3);
				writeChar(4);
				writeChar(7);
				break;
			case 5:
				writeChar(3);
				writeChar(6);
				writeChar(6);
				break;
			case 6:
				writeChar(0);
				writeChar(6);
				writeChar(6);
				break;
			case 7:
				writeChar(1);
				writeChar(1);
				writeChar(2);
				break;
			case 8:
				writeChar(0);
				writeChar(6);
				writeChar(2);
				break;
			case 9:
				writeChar(0);
				writeChar(6);
				writeChar(2);
				break;
		}
		setCursor(pos, 1);
		switch (number) {
			case 0:
				writeChar(3);
				writeChar(4);
				writeChar(5);
				break;
			case 1:
				writeChar(4);
				writeChar(7);
				writeChar(4);
				break;
			case 2:
				writeChar(3);
				writeChar(4);
				writeChar(4);
				break;
			case 3:
				writeChar(4);
				writeChar(4);
				writeChar(5);
				break;
			case 4:
				writeChar(' ');
				writeChar(' ');
				writeChar(7);
				break;
			case 5:
				writeChar(4);
				writeChar(4);
				writeChar(5);
				break;
			case 6:
				writeChar(3);
				writeChar(4);
				writeChar(5);
				break;
			case 7:
				writeChar(' ');
				writeChar(' ');
				writeChar(7);
				break;
			case 8:
				writeChar(3);
				writeChar(4);
				writeChar(5);
				break;
			case 9:
				writeChar(' ');
				writeChar(' ');
				writeChar(7);
				break;
		}
	}

private:
	inline void dataPartWrite(unsigned char dataPart)
	{
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
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_5 | GPIO_Pin_6;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_Write(GPIOB, GPIO_Pin_9);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB, &GPIO_InitStructure);


	uint32_t i = 0;

	// ADC
	ADC_InitTypeDef ADC_InitStructure;
	RCC_ADCCLKConfig(RCC_ADCCLK_PCLK_Div4);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	ADC_DeInit(ADC1);
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_ScanDirection = ADC_ScanDirection_Backward;
	ADC_Init(ADC1, &ADC_InitStructure);
	ADC_Cmd(ADC1, ENABLE);

	ADC_ChannelConfig(ADC1, ADC_Channel_8, ADC_SampleTime_1_5Cycles);
	ADC_StartOfConversion(ADC1);

	LCDDisplay display;
	display.initialize();
	display.createChar(0, LT);
	display.createChar(1, UB);
	display.createChar(2, RT);
	display.createChar(3, LL);
	display.createChar(4, LB);
	display.createChar(5, LR);
	display.createChar(6, MB);
	display.createChar(7, block);

	display.setCursor(0, 0);
	display.writeTemperature(123);

	while (1) {
		//display.setCursor(0, 0);
		int t_sum = 0;
		for (int j = 0; j < 64; ++j) {
			i = ADC_GetConversionValue(ADC1);
			float x = float(i) / U_ANALOG_MAX;
			// zdroj: http://www.mosaic-industries.com/embedded-systems/microcontroller-projects/temperature-measurement/ntc-thermistors/example-code-equations
			float t = 25.0f + (x*(-99.296f + x*(24.624f + x*147.26f))) / (1.0f + x*(0.0195f + x*2.8454f));
			t_sum += int(t * 10);
			delay_us(1000);
		}
		//display.writeNumber(int(t * 10));
		display.writeTemperature(int(t_sum / 64));
		GPIOC->ODR ^= (1 << 9);
	}
}

}
