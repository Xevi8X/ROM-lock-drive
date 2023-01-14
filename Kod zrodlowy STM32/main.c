/**
  ******************************************************************************
  * @file    main.c
  * @author  Wojciech Gajda
  ******************************************************************************
*/


#include "stm32f3xx.h"
#include "stm32f3xx_nucleo_32.h"
			

uint16_t  PA1_Buff = 0x0000;
uint8_t   PA1_state = 0;
uint32_t  PF0_Buff = 0x0000;
uint8_t   PF0_state = 0;
uint32_t  PB1_Buff = 0x0000;
uint8_t   PB1_state = 0;
volatile int i= 0;
uint16_t  counter = 0;
uint16_t cc = 0;
//najmlodszy bit to update ogolny, drugi to update prompt
uint8_t update = 0x00;
uint8_t prompt = 0x00;


/*
uint8_t mosfetOn(uint8_t clockwise);
void mosfetOff();
void autoOff(int16_t delay_in_miliseconds);
void TIM2_IRQHandler(void);
void RRC_init();
void GPIO_init();
void TIM2_init();
*/


//Wylaczenie wszystkich kluczy w mostku H
void mosfetOff()
{
	counter = 0;
	GPIOB -> ODR |= GPIO_ODR_5;
	GPIOB -> ODR &= ~GPIO_ODR_4;
	GPIOA -> ODR |= GPIO_ODR_11;
	GPIOA -> ODR &= ~GPIO_ODR_8;

}

//Wlaczenie obrotow w odpowiednim kierunku, zwraca 1 gdy w momencie wywolania mostek byl w stanie innym niz mosfetOff()
uint8_t mosfetOn(uint8_t clockwise) //clockwise = 1, ccw = 0
{
	if((GPIOB -> ODR & GPIO_ODR_4 ) || (GPIOA -> ODR & GPIO_ODR_8) || !(GPIOB -> ODR & GPIO_ODR_5) || !(GPIOA -> ODR & GPIO_ODR_11) )
	{
		mosfetOff();
		return 1;
	}

	if(clockwise)
	{
		GPIOB -> BSRR |= GPIO_BSRR_BR_5;
		GPIOA -> BSRR |= GPIO_BSRR_BS_8;
	}
	else
	{
		GPIOA -> BSRR |= GPIO_BSRR_BR_11;
		GPIOB -> BSRR |= GPIO_BSRR_BS_4;
	}
	return 0;
}

//Sprawdzenie czy nie przekroczono limitu czasu
void autoOff(int16_t delay_in_miliseconds)
{
	if((GPIOB -> ODR & GPIO_ODR_4 ) || (GPIOA -> ODR & GPIO_ODR_8) || !(GPIOB -> ODR & GPIO_ODR_5) || !(GPIOA -> ODR & GPIO_ODR_11) )
			{
				counter++;
			}

			if(counter > delay_in_miliseconds)
			{
				mosfetOff();
			}
}

//Wyslanie komunikatu
void sendPrompt(uint8_t text)
{
	//przykladowo
	//0x01 jedno migniecie krotkie
	//0x07 jedno dlugie
	//0x55 4 krotkie
	//0x77 dwa dlugie
		update |= 0x02; //bit update prompt
		prompt = text;
}

//wyswietlenie promptu
void showPrompt()
{
	if(prompt == 0x00)
	{
		update &= ~0x02;
	}

	if((prompt & 0x01) == 0x01)
	{
		GPIOB->ODR |= GPIO_ODR_3;
	}
	else
	{
		GPIOB->ODR &= ~GPIO_ODR_3;
	}
		prompt >>= 1;
}


void TIM2_IRQHandler(void) //Wywolanie co 1ms
{
	if (0 != (TIM_SR_UIF & TIM2->SR)) // check update interrupt flag
	{
		TIM2->SR &= ~TIM_SR_UIF; // clear interrupt flag

		PA1_Buff <<=  1;
		PA1_Buff |= (GPIOA->IDR & GPIO_IDR_1)?(0x01):(0x00);
		PF0_Buff <<=  1;
		PF0_Buff |= (GPIOF->IDR & GPIO_IDR_0)?(0x01):(0x00);
		PB1_Buff <<= 1;
		PB1_Buff |= (GPIOB->IDR & GPIO_IDR_1)?(0x01):(0x00);

		update |=  0x01;
		autoOff(1000);
		cc++;
	}
}

//Inicjalizacja zegara HSI i lini zegarowych dla GPIO
void RRC_init()
{
	//--------------------------------------------------------------------------
		//URUCHOMIENIE ZEGARA HSI
		RCC->CR |= RCC_CR_HSION; //uruchomienie szybkiego zegara wewnetrznego 8MHz
		while (0 == (RCC->CR  & RCC_CR_HSIRDY)) {} //oczekiwanie na potwierdzenie uruchomienia
		RCC->CFGR |= RCC_CFGR_SW_0; //Przelaczenie sie na zegar HSI

		//--------------------------------------------------------------------------
		//URUCHOMIENIE PORTOW GPIO
		__HAL_RCC_GPIOA_CLK_ENABLE();
		__HAL_RCC_GPIOB_CLK_ENABLE();
		__HAL_RCC_GPIOC_CLK_ENABLE();
		__HAL_RCC_GPIOD_CLK_ENABLE();
		__HAL_RCC_GPIOF_CLK_ENABLE();

}
//Konfiguracja portów GPIO
void GPIO_init()
{


	//PF1 - Enable L298N
	GPIOF->MODER &= ~GPIO_MODER_MODER1_Msk;   //Bity Mode -> 00
	GPIOF->MODER |= GPIO_MODER_MODER1_0;     // Bity Mode -> 01 PP GP
	GPIOF->OSPEEDR &= ~GPIO_OSPEEDER_OSPEEDR1_Msk; //Low speed
	GPIOF->OTYPER &= ~GPIO_OTYPER_OT_1; //Push pull
	GPIOF->PUPDR &= ~GPIO_PUPDR_PUPDR1_Msk; //zadnych pull up/down
	GPIOF->ODR &= ~GPIO_ODR_1;		  //N-MOSFET, stan niski

	//Pull-up
	//PA1 - Przycisk
	GPIOA->MODER &= ~GPIO_MODER_MODER1_Msk; //Bity Mode -> 00 Input
	GPIOA->PUPDR &= ~GPIO_PUPDR_PUPDR1_Msk;
	GPIOA->PUPDR |= GPIO_PUPDR_PUPDR1_0; //Pull-up

	//PF0 - Krancowka 1
	GPIOF->MODER &= ~GPIO_MODER_MODER0_Msk; //Bity Mode -> 00 Input
	GPIOF->PUPDR &= ~GPIO_PUPDR_PUPDR0_Msk;
	GPIOF->PUPDR |= GPIO_PUPDR_PUPDR0_0; //Pull-up

	//PB1 - Krancowka 2
	GPIOB->MODER &= ~GPIO_MODER_MODER1_Msk; //Bity Mode -> 00 Input
	GPIOB->PUPDR &= ~GPIO_PUPDR_PUPDR1_Msk;
	GPIOB->PUPDR |= GPIO_PUPDR_PUPDR1_0; //Pull-up

	//PB0 - Kontrakton
	GPIOB->MODER &= ~GPIO_MODER_MODER0_Msk; //Bity Mode -> 00 Input
	GPIOB->PUPDR &= ~GPIO_PUPDR_PUPDR0_Msk;
	GPIOB->PUPDR |= GPIO_PUPDR_PUPDR0_0; //Pull-up

	//Push pull
	//PB5 - Mosfet Left-High
	GPIOB->MODER &= ~GPIO_MODER_MODER5_Msk;   //Bity Mode -> 00
	GPIOB->MODER |= GPIO_MODER_MODER5_0;     // Bity Mode -> 01 PP GP
	GPIOB->OSPEEDR &= ~GPIO_OSPEEDER_OSPEEDR5_Msk; //Low speed
	GPIOB->OTYPER &= ~GPIO_OTYPER_OT_5; //Push pull
	GPIOB->PUPDR &= ~GPIO_PUPDR_PUPDR5_Msk; //zadnych pull up/down
	GPIOB -> ODR |= GPIO_ODR_5;		  //P-MOSFET, stan wysoki

	//PB4 - Mosfet Left-Low
	GPIOB->MODER &= ~GPIO_MODER_MODER4_Msk;   //Bity Mode -> 00
	GPIOB->MODER |= GPIO_MODER_MODER4_0;     // Bity Mode -> 01 PP GP
	GPIOB->OSPEEDR &= ~GPIO_OSPEEDER_OSPEEDR4_Msk; //Low speed
	GPIOB->OTYPER &= ~GPIO_OTYPER_OT_4; //Push pull
	GPIOB->PUPDR &= ~GPIO_PUPDR_PUPDR4_Msk; //zadnych pull up/down
	GPIOB -> ODR &= ~GPIO_ODR_4;		  //N-MOSFET, stan niski

	//PA11 - Mosfet Right-High
	GPIOA->MODER &= ~GPIO_MODER_MODER11_Msk;   //Bity Mode -> 00
	GPIOA->MODER |= GPIO_MODER_MODER11_0;     // Bity Mode -> 01 PP GP
	GPIOA->OSPEEDR &= ~GPIO_OSPEEDER_OSPEEDR11_Msk; //Low speed
	GPIOA->OTYPER &= ~GPIO_OTYPER_OT_11; //Push pull
	GPIOA->PUPDR &= ~GPIO_PUPDR_PUPDR11_Msk; //zadnych pull up/down
	GPIOA -> ODR |= GPIO_ODR_11;		  //P-MOSFET, stan wysoki

	//PA8 - Mosfet Right-Low
	GPIOA->MODER &= ~GPIO_MODER_MODER8_Msk;   //Bity Mode -> 00
	GPIOA->MODER |= GPIO_MODER_MODER8_0;     // Bity Mode -> 01 PP GP
	GPIOA->OSPEEDR &= ~GPIO_OSPEEDER_OSPEEDR8_Msk; //Low speed
	GPIOA->OTYPER &= ~GPIO_OTYPER_OT_8; //Push pull
	GPIOA->PUPDR &= ~GPIO_PUPDR_PUPDR8_Msk; //zadnych pull up/down
	GPIOA -> ODR &= ~GPIO_ODR_8;		  //N-MOSFET, stan niski

	//PB3 - Dioda
	GPIOB->MODER &= ~GPIO_MODER_MODER3_Msk;   //Bity Mode -> 00
	GPIOB->MODER |= GPIO_MODER_MODER3_0;     // Bity Mode -> 01 PP GP
	GPIOB->OSPEEDR &= ~GPIO_OSPEEDER_OSPEEDR3_Msk; //Low speed
	GPIOB->OTYPER &= ~GPIO_OTYPER_OT_3; //Push pull
	GPIOB->PUPDR &= ~GPIO_PUPDR_PUPDR1_Msk; //zadnych pull up/down
	GPIOB->ODR &= ~GPIO_ODR_3;		  //domyslnie enable

	GPIOF->ODR |= GPIO_ODR_1;
}

//Konf. timera
void TIM2_init()
{
	//--------------------------------------------------------------------------
		//KONFIGURACJA TIMERA TIM2 - przerwanie co 1ms
		RCC->APB1ENR |= RCC_APB1ENR_TIM2EN; //uruchomienie linii zegarowej
		//8,000,000Hz / 1000Hz = 8000, 8000/800 = 10
		TIM2->PSC = 64 - 1; //konfiguracja prescalera  w zakresie od 0 do 65535 + 1
		TIM2->ARR = 125 - 1; //i rejestru autoreload
		TIM2->DIER |= TIM_DIER_UIE; //uruchomienie update interrupt
		//Zostawiamy na frozen, nie bedziemy korzystac z funkcjonalnosci PWM
		TIM2->CCMR1 &= ~TIM_CCMR1_OC1M_Msk;

		//--------------------------------------------------------------------------
		//URUCHOMINIE MODULU PRZERWAN NVIC DLA TIM2
		NVIC_EnableIRQ(TIM2_IRQn);

		//--------------------------------------------------------------------------
		//URUCHOMINIE TIM2
		TIM2->CR1 |= TIM_CR1_CEN;

		//Komunikat o poprawnej inicjalizacji
		sendPrompt(0x15); // 3 krotkie
}


int main(void)
{

 	RRC_init();
	GPIO_init();
	TIM2_init();

	for(;;)
	{
		if((update & 0x01) == 0x01)
		{
			__disable_irq();
			if((PA1_Buff == 0xFFFF) && PA1_state == 0)
			{
				PA1_state = 1;
			}

			if((PA1_Buff == 0x0000) && PA1_state == 1)
			{
				PA1_state = 0;
				if((GPIOB->IDR & GPIO_IDR_0 ) == 0)
				{
					if((GPIOF->IDR & GPIO_IDR_0) == 0)
					{
					sendPrompt(0x01);
					mosfetOn(0);
					}
					else
					{
					sendPrompt(0x05);
					mosfetOn(1);
					}
				}
				else
				{
					sendPrompt(0xE7);
					mosfetOn(1);
				}
			}

			if((PF0_Buff == 0xFFFFFFFF) && PF0_state == 0)
			{
				PF0_state = 1;
			}

			if((PF0_Buff == 0x00000000) && PF0_state == 1)
			{
				PF0_state = 0;
				counter = 800;
			}

			if((PB1_Buff == 0xFFFFFFFF) && PB1_state == 0)
			{
				PB1_state = 1;
			}

			if((PB1_Buff == 0x00000000) && PB1_state == 1)
			{
				PB1_state = 0;
				counter = 800;
			}

			if(cc > 200)
			{
				if((update & 0x02) == 0x02)
				{
					showPrompt();
				}
				cc = 0;
			}

			update &= ~0x01;
			__enable_irq();
		}
	}
}
