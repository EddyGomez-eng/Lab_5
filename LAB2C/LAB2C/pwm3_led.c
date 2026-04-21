#include "pwm3_led.h"                // Incluye el header de la librería propia del LED
#include <avr/interrupt.h>           // Permite usar interrupciones y la rutina ISR

volatile uint8_t ct3 = 0;            // Lleva la cuenta interna de pasos dentro del periodo PWM
volatile uint8_t dc3 = 0;            // Guarda el duty cycle pedido en porcentaje
volatile uint8_t pd3 = 0;            // Guarda el duty cycle convertido a pasos reales del PWM

void PWM3_led_init(void)             // Esta rutina deja listo el LED y el Timer0
{
	DDRD |= (1 << DDD3);             // Configura D3 como salida para controlar el LED
	PORTD &= ~(1 << PORTD3);         // Deja D3 en LOW para arrancar con el LED apagado

	TCCR0A = 0;                      // Limpia la configuración previa del Timer0 parte A
	TCCR0B = 0;                      // Limpia la configuración previa del Timer0 parte B
	TCNT0 = 0;                       // Reinicia el contador interno del Timer0 desde cero

	TCCR0A |= (1 << WGM01);          // Activa modo CTC para reiniciar el timer al llegar a OCR0A
	OCR0A = 79;                      // Baja el valor de comparación para que el PWM manual sea más rápido

	TCCR0B |= (1 << CS01);           // Coloca prescaler 8 para que el timer avance a buena velocidad
	TIMSK0 |= (1 << OCIE0A);         // Habilita la interrupción cuando Timer0 llegue a OCR0A

	dc3 = 0;                         // Arranca el duty cycle en 0 %
	pd3 = 0;                         // Arranca también en 0 pasos encendidos
}

void PWM3_led_set_duty(uint8_t duty) // Esta rutina recibe el duty cycle en porcentaje
{
	if (duty > 100)                 // Si el valor supera el máximo permitido
	{
		duty = 100;                 // Lo limita a 100 %
	}

	dc3 = duty;                     // Guarda el porcentaje original
	pd3 = ((uint16_t)duty * 64U) / 100U; // Convierte 0-100 % a un rango de 0-64 pasos PWM
}

void PWM3_led_set_from_adc(uint16_t adc_value) // Convierte lectura ADC en brillo del LED
{
	uint8_t dt = 0;                 // Aquí se guardará el porcentaje final calculado

	if (adc_value <= 25)            // Si el potenciómetro está casi en el mínimo
	{
		dt = 0;                     // Fuerza apagado total para evitar brillo residual
	}
	else if (adc_value >= 995)      // Si el potenciómetro está casi al máximo
	{
		dt = 100;                   // Fuerza encendido total
	}
	else                             // Si está dentro del rango normal
	{
		dt = ((uint32_t)adc_value * 100UL) / 1023UL; // Convierte 0-1023 a 0-100 %
	}

	PWM3_led_set_duty(dt);          // Envía ese porcentaje al generador PWM
}

ISR(TIMER0_COMPA_vect)              // Esta interrupción se ejecuta cada vez que Timer0 compara
{
	ct3++;                          // Avanza un paso dentro del periodo PWM actual

	if (ct3 >= 64)                 // Si ya completó los 64 pasos del periodo
	{
		ct3 = 0;                   // Reinicia el contador para comenzar otro ciclo
		PORTD |= (1 << PORTD3);    // Enciende el LED al inicio del nuevo ciclo
	}

	if (pd3 == 0)                  // Si el duty cycle vale 0 %
	{
		PORTD &= ~(1 << PORTD3);   // Mantiene el LED completamente apagado
	}
	else if (pd3 >= 64)            // Si el duty cycle vale 100 %
	{
		PORTD |= (1 << PORTD3);    // Mantiene el LED completamente encendido
	}
	else if (ct3 >= pd3)           // Cuando el contador llega al tiempo encendido pedido
	{
		PORTD &= ~(1 << PORTD3);   // Apaga el LED por el resto del ciclo
	}
}