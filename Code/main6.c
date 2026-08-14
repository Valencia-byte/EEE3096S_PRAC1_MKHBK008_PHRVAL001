/*
 * Task 7: Software PWM via Timer Interrupts
 * Target: STM32F051C8 (UCT Dev Board)
 * Output: PB5 (Byte of LEDs bit D5)
 * Signal: 100 Hz, 30% duty cycle, generated entirely in software
 */
#include "stm32f0xx.h"

#define TIM16_PSC_VALUE   7
#define TIM16_ARR_VALUE   99

static void GPIO_Init(void);
static void TIM16_Init(void);

int main(void)
{
    /* TODO: Initialize GPIO and TIM16 peripherals */
	GPIO_Init();
	TIM16_Init();
    while (1)
    {
        /* All PWM generation happens in the ISR; main loop remains free */
    }
}

static void GPIO_Init(void)
{
    /* TODO: Enable GPIOB clock */

    /* TODO: Configure PB5 as a general purpose output, push-pull, medium speed, no pull-up/pull-down */


    /* TODO: Ensure PB5 starts low */

    /* Enable GPIOB clock */
        RCC->AHBENR |= RCC_AHBENR_GPIOBEN;

        /* Configure PB5 as general purpose output (01) */
        GPIOB->MODER &= ~(GPIO_MODER_MODER4_Msk);
        GPIOB->MODER |= (1U << GPIO_MODER_MODER4_Pos);

        /* Push-pull output (0) */
        GPIOB->OTYPER &= ~(GPIO_OTYPER_OT_4);

        /* Medium speed (01) */
        GPIOB->OSPEEDR &= ~(GPIO_OSPEEDR_OSPEEDR4_Msk);
        GPIOB->OSPEEDR |= (1U << GPIO_OSPEEDR_OSPEEDR4_Pos);

        /* No pull-up, no pull-down (00) */
        GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPDR4_Msk);

        /* Ensure PB5 starts low */
        GPIOB->BRR = GPIO_BRR_BR_4;
}

static void TIM16_Init(void)
{
    /* TODO: Enable TIM16 clock */
	RCC->APB2ENR |= RCC_APB2ENR_TIM16EN;
    /* TODO: Set Prescaler and ARR values for 10 kHz interrupt frequency */
	TIM16->PSC = TIM16_PSC_VALUE;
	TIM16->ARR = TIM16_ARR_VALUE;
    /* TODO: Clear any pending update flag and enable the update interrupt (DIER) */
	TIM16->SR &= ~TIM_SR_UIF;
	TIM16->DIER |= TIM_DIER_UIE;
    /* TODO: Configure NVIC priority and enable TIM16 interrupt */
	NVIC_SetPriority(TIM16_IRQn, 0);
	NVIC_EnableIRQ(TIM16_IRQn);
    /* TODO: Enable the counter to start the interrupt stream */
	TIM16->CR1 |= TIM_CR1_CEN;
}

void TIM16_IRQHandler(void)
{
    static uint8_t counter = 0;

    /* TODO: Check if the update interrupt flag (UIF) is set */
    if (TIM16->SR & TIM_SR_UIF) {
        /* TODO: Clear the update interrupt flag */
        TIM16->SR &= ~TIM_SR_UIF;
        /* TODO: Implement 30% duty cycle logic:
         * If counter < 30, drive PB5 high using BSRR.
         * If counter >= 30, drive PB5 low using BRR/BSRR. */
        if (counter < 30) {
        	GPIOB->BSRR = GPIO_BSRR_BS_4;
        }else {
        	GPIOB->BSRR = GPIO_BSRR_BR_4;
        }

        /* TODO: Increment counter and reset to zero when counter reaches 100 */
        counter++;
        if (counter == 100) {
        	counter = 0;
        }
	}
}