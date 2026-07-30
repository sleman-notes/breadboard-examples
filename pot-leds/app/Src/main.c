#include <stdio.h>
#include <stdint.h>
#include "config.h"
#include "driver_gpio.h"
#include "driver_adc.h"
#include "driver_systick.h"

#define LED_RED             GPIO_PIN_NO_3
#define LED_YELLOW          GPIO_PIN_NO_4
#define LED_GREEN           GPIO_PIN_NO_5

#define BLINK_PERIOD_MS     500
#define PRINT_PERIOD_MS     500

int main(void)
{
    uint64_t now;
    uint64_t blink_last = 0;
    uint64_t print_last = 0;
    uint32_t raw;

    config_app();

    setvbuf(stdout, NULL, _IONBF, 0);

    led_init(GPIOB, LED_RED);
    led_init(GPIOB, LED_YELLOW);
    led_init(GPIOB, LED_GREEN);

    adc_pa1_init();
    adc_start_conversion();

    while(1)
    {
        now = ticks_get();

        if((now - blink_last) >= BLINK_PERIOD_MS)
        {
            GPIO_ToggleOutputPin(GPIOB, LED_RED);
            GPIO_ToggleOutputPin(GPIOB, LED_YELLOW);
            GPIO_ToggleOutputPin(GPIOB, LED_GREEN);
            blink_last = now;
        }

        if((now - print_last) >= PRINT_PERIOD_MS)
        {
            raw = adc_read();
            printf("pot: %u\r\n", (unsigned int)raw);
            print_last = now;
        }
    }
}
