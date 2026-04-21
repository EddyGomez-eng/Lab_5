#ifndef PWM2_SERVO_H_                // Evita que este header se incluya dos veces por error
#define PWM2_SERVO_H_                // Marca de protección del archivo

#include <avr/io.h>                  // Trae los registros del microcontrolador
#include <stdint.h>                  // Permite usar tipos como uint16_t

void PWM2_servo_init(void);          // Declara la rutina que configura el PWM2 para el servo 2
void PWM2_servo_set_pulse_us(uint16_t us); // Declara la rutina que fija el pulso del servo 2 en microsegundos
void PWM2_servo_set_from_adc(uint16_t adc_value); // Declara la rutina que convierte una lectura ADC en pulso para el servo 2

#endif                               // Cierra la protección del archivo