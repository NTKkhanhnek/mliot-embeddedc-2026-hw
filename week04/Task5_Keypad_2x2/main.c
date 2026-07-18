#include "stm32f1xx_hal.h"
#include <stdint.h>

/*
 * TASK 5 - KEYPAD 2x2
 *
 * Row1 = PA0
 * Row2 = PA1
 * Col1 = PB0
 * Col2 = PB1
 * LED  = PC13 (active-low)
 *
 * Chọn CÁCH 2: LED điều khiển bằng firmware.
 *
 * Sơ đồ logic:
 *
 *             Col1(PB0)   Col2(PB1)
 *                |           |
 * Row1(PA0) ---- SW1 ------- SW2
 *                |           |
 * Row2(PA1) ---- SW3 ------- SW4
 *
 * Phím 1 = Row1-Col1
 * Phím 2 = Row1-Col2
 * Phím 3 = Row2-Col1
 * Phím 4 = Row2-Col2
 */

#define ROW1_PORT GPIOA
#define ROW1_PIN  GPIO_PIN_0
#define ROW2_PORT GPIOA
#define ROW2_PIN  GPIO_PIN_1

#define COL1_PORT GPIOB
#define COL1_PIN  GPIO_PIN_0
#define COL2_PORT GPIOB
#define COL2_PIN  GPIO_PIN_1

#define LED_PORT  GPIOC
#define LED_PIN   GPIO_PIN_13

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

    /* BƯỚC 1: Enable Clock GPIOA, GPIOB, GPIOC trên APB2. */
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;

    /* Row1 PA0, Row2 PA1: Output Push-Pull. */
    gpio.Pin = ROW1_PIN | ROW2_PIN;
    gpio.Mode = GPIO_MODE_OUTPUT_PP;
    gpio.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &gpio);

    HAL_GPIO_WritePin(ROW1_PORT, ROW1_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(ROW2_PORT, ROW2_PIN, GPIO_PIN_SET);

    /* Col1 PB0, Col2 PB1: Input Pull-up. */
    gpio.Pin = COL1_PIN | COL2_PIN;
    gpio.Mode = GPIO_MODE_INPUT;
    gpio.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOB, &gpio);

    /* PC13: LED Output Push-Pull. */
    gpio.Pin = LED_PIN;
    gpio.Mode = GPIO_MODE_OUTPUT_PP;
    gpio.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(LED_PORT, &gpio);

    HAL_GPIO_WritePin(LED_PORT, LED_PIN, GPIO_PIN_SET);
}

/*
 * Quét thô keypad.
 *
 * Trả về:
 * 0: Không có phím
 * 1: Row1-Col1
 * 2: Row1-Col2
 * 3: Row2-Col1
 * 4: Row2-Col2
 */
static uint8_t keypad_scan_raw(void)
{
    /* Mặc định đưa cả hai hàng lên HIGH. */
    HAL_GPIO_WritePin(ROW1_PORT, ROW1_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(ROW2_PORT, ROW2_PIN, GPIO_PIN_SET);

    /* Quét Row1: kéo Row1 xuống LOW, đọc 2 cột. */
    HAL_GPIO_WritePin(ROW1_PORT, ROW1_PIN, GPIO_PIN_RESET);
    __NOP();
    __NOP();

    if (HAL_GPIO_ReadPin(COL1_PORT, COL1_PIN) == GPIO_PIN_RESET)
    {
        HAL_GPIO_WritePin(ROW1_PORT, ROW1_PIN, GPIO_PIN_SET);
        return 1U;
    }

    if (HAL_GPIO_ReadPin(COL2_PORT, COL2_PIN) == GPIO_PIN_RESET)
    {
        HAL_GPIO_WritePin(ROW1_PORT, ROW1_PIN, GPIO_PIN_SET);
        return 2U;
    }

    HAL_GPIO_WritePin(ROW1_PORT, ROW1_PIN, GPIO_PIN_SET);

    /* Quét Row2: kéo Row2 xuống LOW, đọc 2 cột. */
    HAL_GPIO_WritePin(ROW2_PORT, ROW2_PIN, GPIO_PIN_RESET);
    __NOP();
    __NOP();

    if (HAL_GPIO_ReadPin(COL1_PORT, COL1_PIN) == GPIO_PIN_RESET)
    {
        HAL_GPIO_WritePin(ROW2_PORT, ROW2_PIN, GPIO_PIN_SET);
        return 3U;
    }

    if (HAL_GPIO_ReadPin(COL2_PORT, COL2_PIN) == GPIO_PIN_RESET)
    {
        HAL_GPIO_WritePin(ROW2_PORT, ROW2_PIN, GPIO_PIN_SET);
        return 4U;
    }

    HAL_GPIO_WritePin(ROW2_PORT, ROW2_PIN, GPIO_PIN_SET);

    return 0U;
}

/*
 * Debounce keypad.
 * Hàm được gọi mỗi 1 ms.
 * Chỉ trả về mã phím một lần khi trạng thái nhấn đã ổn định khoảng 20 ms.
 * Giữ phím không tạo sự kiện liên tục.
 */
static uint8_t keypad_get_event(void)
{
    static uint8_t last_raw = 0U;
    static uint8_t stable_key = 0U;
    static uint8_t stable_count = 0U;

    uint8_t raw = keypad_scan_raw();

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

    if ((stable_count >= 20U) && (stable_key != raw))
    {
        stable_key = raw;

        if (stable_key != 0U)
        {
            return stable_key;
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
        uint8_t key = keypad_get_event();

        /*
         * Chọn phím mục tiêu là phím 1 (Row1-Col1).
         * Mỗi lần nhấn hợp lệ, LED đảo trạng thái một lần.
         */
        if (key == 1U)
        {
            led_on = !led_on;

            HAL_GPIO_WritePin(
                LED_PORT,
                LED_PIN,
                led_on ? GPIO_PIN_RESET : GPIO_PIN_SET
            );
        }

        delay_ms(1);
    }
}
