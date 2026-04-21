#ifndef PWM3_LED_H_                  // Evita que este header se incluya dos veces por error
#define PWM3_LED_H_                  // Marca de protección del archivo

#include <avr/io.h>                  // Trae los registros del microcontrolador
#include <stdint.h>                  // Permite usar tipos como uint8_t y uint16_t

void PWM3_led_init(void);            // Declara la rutina que deja listo el PWM manual del LED
void PWM3_led_set_duty(uint8_t duty); // Declara la rutina que fija el duty cycle de 0 a 100
void PWM3_led_set_from_adc(uint16_t adc_value); // Declara la rutina que convierte una lectura ADC en duty cycle

#endif                               // Cierra la protección del archivo