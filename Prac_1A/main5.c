#include "main.h"

/* ---------- Function prototypes ---------- */
static void GPIO_Init(void);
static void ADC_Init(void);
static void write_led_byte(uint8_t pattern);

GPIO_TypeDef* led_ports[8] = {GPIOB, GPIOB, GPIOB, GPIOB,
GPIOB, GPIOB, GPIOB, GPIOB};
uint16_t led_pins[8] = {GPIO_PIN_0, GPIO_PIN_1, GPIO_PIN_2, GPIO_PIN_3,
GPIO_PIN_4, GPIO_PIN_5, GPIO_PIN_6, GPIO_PIN_7};

int main(void)
{
    HAL_Init(); // still fine to call - only sets up SysTick/core clocks
    GPIO_Init();
    ADC_Init();

    /* TODO: Start continuous conversion using ADC1 control register */
    ADC1->CR |= ADC_CR_ADSTART;

    while (1)
    {
        /* everything happens in the ISR */
    }
}

/* ---------- GPIO Configuration ---------- */
static void GPIO_Init(void)
{
    /* TODO: Enable clocks for GPIOA and GPIOB */
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

    /* TODO: Configure PA5 as Analog mode (11) for ADC_IN5 */
	GPIOA -> MODER |= GPIO_MODER_MODER5;
    /* TODO: Configure PB0-PB7 as General purpose outputs (01) and clear ODR initially */
	GPIOB->ODR = 0x00;//clears the register
	GPIOB->MODER &= ~(0x00005555);
	GPIOB->MODER |= 0x00005555;//sets the pbo-pb7 as outputs

}


/* ---------- ADC Configuration ---------- */
static void ADC_Init(void)
{
    /* TODO: Enable APB2 clock for ADC1 */
	RCC->APB2ENR |= RCC_APB2ENR_ADCEN;
    /* TODO: Ensure ADC is disabled, then run calibration */
	if (ADC1->CR & ADC_CR_ADEN) {               // Check if ADC is enabled
	        ADC1->CR |= ADC_CR_ADDIS;               // Disable ADC
	        while (ADC1->CR & ADC_CR_ADEN);         // Wait until ADC is disabled
	}
	ADC1->CR |= ADC_CR_ADCAL;                   // Start calibration
	while (ADC1->CR & ADC_CR_ADCAL);            // Wait until calibration is complete
    /* TODO: Select channel 5 (PA5) and configure sampling time */
	ADC1->CHSELR = ADC_CHSELR_CHSEL5;
	ADC1->SMPR = 2U;//13.5 ADC cycles
    /* TODO: Configure 12-bit resolution, right alignment, and continuous conversion mode */
	ADC1->CFGR1 &= ~ADC_CFGR1_ALIGN;
	ADC1->CFGR1 &= ~ADC_CFGR1_RES;
	ADC1->CFGR1 |= ADC_CFGR1_CONT;
    /* TODO: Enable end-of-conversion interrupt and configure NVIC priorities */
	ADC1->IER |= ADC_IER_EOCIE;
	NVIC_SetPriority(ADC1_IRQn, 1);//setting up the NCIC properties
	NVIC_EnableIRQ(ADC1_IRQn);
    /* TODO: Enable the ADC and wait for the ADRDY flag */
	ADC1->CR |= ADC_CR_ADEN;                // Enable ADC
	while (!(ADC1->ISR & ADC_ISR_ADRDY));
}

/* ---------- Interrupt Service Routine ---------- */
void ADC1_COMP_IRQHandler(void)
{
    if (ADC1->ISR & ADC_ISR_EOC)
    {
        /* TODO: Read the raw 12-bit ADC value from the data register (DR) */
        uint32_t adc_val = ADC1->DR; // Replace with register read

        /* TODO: Scale the raw value proportionally to a number of LEDs (0 - 8) */
        uint8_t leds_to_light = (adc_val * 8) / 4095; // Replace with your scaling calculation

        /* TODO: Build the bar-graph bit pattern using bitwise shifting */
        uint8_t output_pattern = (1U << leds_to_light)-1; // Replace with your bit pattern logic

        /* TODO: Drive PB0-PB7 using the helper function */
        write_led_byte(output_pattern);
    }
}

/* ---------- Helper: write an 8-bit pattern to PB0-PB7 ---------- */
static void write_led_byte(uint8_t pattern)
{
    uint32_t odr = GPIOB->ODR;
    odr &= ~(0xFFU);
    odr |= pattern;
    GPIOB->ODR = odr;
}
