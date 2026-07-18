#include "stm32f1xx_hal.h"
#include <stdint.h>

/*
 * TASK 3 - LED BLINK
 * STM32F103C8T6 Blue Pill
 * LED onboard PC13 là active-low:
 * RESET = LED sáng, SET = LED tắt.
 */

static void delay_init(void)
{
    SysTick->LOAD = (SystemCoreClock / 1000U) - 1U;
    SysTick->VAL = 0U;
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk |
                    SysTick_CTRL_ENABLE_Msk;
}

static void delay_ms(uint32_t ms)
{
    while (ms--)
    {
        while ((SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) == 0U)
        {
        }
    }
}

static void gpio_init(void)
{
    GPIO_InitTypeDef gpio = {0};

    /* BƯỚC 1: Bật Clock GPIOC trên APB2. */
    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;

    /* BƯỚC 2: Cấu hình PC13 Output Push-Pull. */
    gpio.Pin = GPIO_PIN_13;
    gpio.Mode = GPIO_MODE_OUTPUT_PP;
    gpio.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOC, &gpio);

    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
}

int main(void)
{
    delay_init();
    gpio_init();

    while (1)
    {
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);
        delay_ms(500);

        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
        delay_ms(500);
    }
}
