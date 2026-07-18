#include "stm32f1xx_hal.h"
#include <stdint.h>

/*
 * TASK 4 - BUTTON DEBOUNCE
 * LED onboard: PC13 (active-low)
 * Button: PA0 nối xuống GND, cấu hình Input Pull-up
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

    /* BƯỚC 1: Bật Clock GPIOA và GPIOC trên APB2. */
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;

    /* PC13: Output Push-Pull cho LED. */
    gpio.Pin = GPIO_PIN_13;
    gpio.Mode = GPIO_MODE_OUTPUT_PP;
    gpio.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOC, &gpio);

    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);

    /* PA0: Input Pull-up cho nút nhấn. */
    gpio.Pin = GPIO_PIN_0;
    gpio.Mode = GPIO_MODE_INPUT;
    gpio.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOA, &gpio);
}

/*
 * Gọi hàm mỗi 1 ms.
 * Trả về 1 đúng một lần sau khi nút đã được nhấn ổn định rồi thả ổn định.
 * Nhấn giữ không tạo thêm sự kiện.
 */
static uint8_t button_released_event(void)
{
    static GPIO_PinState last_raw = GPIO_PIN_SET;
    static GPIO_PinState stable_state = GPIO_PIN_SET;
    static uint8_t stable_count = 0U;
    static uint8_t had_valid_press = 0U;

    GPIO_PinState raw = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0);

    if (raw == last_raw)
    {
        if (stable_count < 20U)
        {
            stable_count++;
        }
    }
    else
    {
        last_raw = raw;
        stable_count = 0U;
    }

    if ((stable_count >= 20U) && (stable_state != raw))
    {
        stable_state = raw;

        if (stable_state == GPIO_PIN_RESET)
        {
            had_valid_press = 1U;
        }
        else if (had_valid_press)
        {
            had_valid_press = 0U;
            return 1U;
        }
    }

    return 0U;
}

int main(void)
{
    uint8_t led_on = 0U;

    delay_init();
    gpio_init();

    while (1)
    {
        if (button_released_event())
        {
            led_on = !led_on;

            HAL_GPIO_WritePin(
                GPIOC,
                GPIO_PIN_13,
                led_on ? GPIO_PIN_RESET : GPIO_PIN_SET
            );
        }

        delay_ms(1);
    }
}
