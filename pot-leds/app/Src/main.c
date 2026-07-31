#include <stdio.h>
#include <stdint.h>
#include "config.h"
#include "driver_gpio.h"
#include "driver_adc.h"
#include "driver_systick.h"

#define LED_BOARD           GPIO_PIN_NO_13      /* PC13, aceso em nivel baixo */

#define LED_RED             GPIO_PIN_NO_3
#define LED_YELLOW          GPIO_PIN_NO_4
#define LED_GREEN           GPIO_PIN_NO_5

#define BLINK_PERIOD_MS     500
#define PRINT_PERIOD_MS     500

/*
 * A escala do ADC (0..4095) e dividida em quatro faixas: nenhum led, verde,
 * verde+amarelo e os tres. Cada limiar tem uma histerese para o led nao ficar
 * piscando quando o potenciometro para bem em cima da transicao.
 */
#define ADC_STEP            1024
#define ADC_HYST            100

static uint8_t pot_level(uint32_t raw, uint8_t level)
{
    while((level < 3) && (raw >= ((level + 1) * ADC_STEP)))
    {
        level++;
    }

    while((level > 0) && (raw < ((level * ADC_STEP) - ADC_HYST)))
    {
        level--;
    }

    return level;
}

int main(void)
{
    uint64_t now;
    uint64_t blink_last = 0;
    uint64_t print_last = 0;
    uint32_t raw;
    uint8_t level = 0;

    config_app();

    setvbuf(stdout, NULL, _IONBF, 0);

    led_init(GPIOC, LED_BOARD);
    GPIO_WriteToOutputPin(GPIOC, LED_BOARD, GPIO_PIN_SET);

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
            GPIO_ToggleOutputPin(GPIOC, LED_BOARD);
            blink_last = now;
        }

        raw = adc_read();
        level = pot_level(raw, level);

        GPIO_WriteToOutputPin(GPIOB, LED_GREEN,  (level >= 1) ? GPIO_PIN_SET : GPIO_PIN_RESET);
        GPIO_WriteToOutputPin(GPIOB, LED_YELLOW, (level >= 2) ? GPIO_PIN_SET : GPIO_PIN_RESET);
        GPIO_WriteToOutputPin(GPIOB, LED_RED,    (level >= 3) ? GPIO_PIN_SET : GPIO_PIN_RESET);

        if((now - print_last) >= PRINT_PERIOD_MS)
        {
            printf("pot: %u | nivel: %u\r\n", (unsigned int)raw, (unsigned int)level);
            print_last = now;
        }
    }
}
