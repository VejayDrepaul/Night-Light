#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"

#define LED_PIN           0
#define BUTTON_PIN        16
#define PHOTORESISTOR_PIN 26
#define PHOTORESISTOR_ADC 0

void photoresistor_adc(uint16_t *adc_val, float *mili_volt) {
    // init pico for adc 
    adc_init();
    adc_gpio_init(PHOTORESISTOR_PIN);
    adc_select_input(PHOTORESISTOR_ADC);

    // 12-bit conversion, assume max value == ADC_VREF == 3.3 V
    const float conv_factor = 3.3f / (1 << 12);
    *adc_val = adc_read();
    *mili_volt = (*adc_val * conv_factor) * 1000;
}

int main() {
    stdio_init_all();

	// init led, photoresistor and button
	cyw43_arch_init();
	gpio_init(LED_PIN);
	gpio_set_dir(LED_PIN, GPIO_OUT);
	gpio_init(BUTTON_PIN);
	gpio_set_dir(BUTTON_PIN, GPIO_IN);
	gpio_pull_up(BUTTON_PIN);


    bool button_state = false;
    while (1) {
        float volt;
        uint16_t resistor_adc;
        photoresistor_adc(&resistor_adc, &volt);

        printf("%.2f mV: ", volt);
        if (volt >= 65) {
            gpio_put(LED_PIN, 1);
            cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
            printf("LEDs active\n");
        }
        else if (volt < 63) {
            gpio_put(LED_PIN, 0);
            cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
            printf("LEDs inactive\n");
        }

        if (gpio_get(BUTTON_PIN) == 0) {
            sleep_ms(250);
            button_state = true;
            while (button_state) {
                gpio_put(LED_PIN, 1);
                cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
                printf("LEDs active\n");

                if (gpio_get(BUTTON_PIN) == 0)
                    button_state = false;
            }
            gpio_put(LED_PIN, 0);
            cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
            printf("LEDs inactive\n");
        }

        sleep_ms(250);
    }
		

	
    return 0;
}
