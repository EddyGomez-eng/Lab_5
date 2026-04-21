#include "pwm2_servo.h"              // Incluye el header de la librería propia del servo 2

void PWM2_servo_init(void)           // Aquí configuramos el Timer2 para controlar el servo 2
{
	DDRB |= (1 << DDB3);             // Pone PB3 como salida, que en Arduino Nano corresponde a D11

	TCCR2A = 0;                      // Limpia la configuración previa del Timer2 parte A
	TCCR2B = 0;                      // Limpia la configuración previa del Timer2 parte B

	TCCR2A |= (1 << COM2A1);         // Hace que OC2A trabaje en modo no invertido para sacar PWM por D11
	TCCR2A |= (1 << WGM20) | (1 << WGM21); // Pone Timer2 en Fast PWM con tope fijo de 255

	TCCR2B |= (1 << CS22) | (1 << CS21) | (1 << CS20); // Usa prescaler 1024 para bajar la frecuencia y acercarla a la del servo

	OCR2A = 23;                      // Deja el servo 2 con un valor inicial aproximado al centro
}

void PWM2_servo_set_pulse_us(uint16_t us) // Esta rutina adapta microsegundos a la escala más limitada del Timer2
{
	uint8_t tk = 0;                 // Aquí guardaremos el valor final que irá a OCR2A

	if (us < 1000)                 // Si el pulso pedido es menor al mínimo útil
	{
		us = 1000;                 // Lo fijamos al mínimo
	}

	if (us > 2000)                 // Si el pulso pedido es mayor al máximo útil
	{
		us = 2000;                 // Lo fijamos al máximo
	}

	tk = (uint8_t)(us / 64);       // Convierte microsegundos a pasos de Timer2, donde cada paso vale aprox 64 us

	if (tk < 16)                   // Si quedó por debajo del rango útil
	{
		tk = 16;                   // Lo sujetamos al límite inferior
	}

	if (tk > 31)                   // Si quedó por encima del rango útil
	{
		tk = 31;                   // Lo sujetamos al límite superior
	}

	OCR2A = tk;                    // Carga ese valor al registro que controla el PWM del servo 2 por D11
}

void PWM2_servo_set_from_adc(uint16_t adc_value) // Esta rutina convierte una lectura ADC en pulso para el servo 2
{
	uint16_t ps = 0;               // Aquí guardaremos el pulso final en microsegundos

	ps = 1000 + ((uint32_t)adc_value * 1000UL) / 1023UL; // Mapea el rango 0-1023 del ADC a 1000-2000 us
	PWM2_servo_set_pulse_us(ps);   // Manda ese pulso ya adaptado al servo 2
}