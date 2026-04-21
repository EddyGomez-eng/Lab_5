#include "pwm1_servo.h"              // Incluye el header de la librería propia del servo 1

void PWM1_servo_init(void)           // Aquí configuramos el Timer1 para controlar el servo 1
{
	DDRB |= (1 << DDB1);             // Pone PB1 como salida, que en Arduino Nano corresponde a D9

	TCCR1A = 0;                      // Limpia la configuración previa del Timer1 parte A
	TCCR1B = 0;                      // Limpia la configuración previa del Timer1 parte B

	TCCR1A |= (1 << COM1A1);         // Hace que OC1A trabaje en modo no invertido para sacar el PWM por D9
	TCCR1A |= (1 << WGM11);          // Activa parte del modo Fast PWM en Timer1
	TCCR1B |= (1 << WGM12) | (1 << WGM13); // Completa el modo Fast PWM usando ICR1 como tope

	TCCR1B |= (1 << CS11);           // Coloca prescaler 8 para lograr un periodo útil para servo

	ICR1 = 39999;                    // Fija un periodo de 20 ms, que equivale a 50 Hz para el servo
	OCR1A = 3000;                    // Deja el servo 1 arrancando cerca del centro con 1500 us
}

void PWM1_servo_set_pulse_us(uint16_t us) // Esta rutina convierte microsegundos en cuentas de Timer1
{
	uint16_t tk = 0;                 // Aquí guardaremos cuántos ticks necesita el pulso

	if (us < 1000)                  // Si el pulso pedido es menor al mínimo seguro
	{
		us = 1000;                  // Lo limitamos al valor mínimo
	}

	if (us > 2000)                  // Si el pulso pedido es mayor al máximo seguro
	{
		us = 2000;                  // Lo limitamos al valor máximo
	}

	tk = us * 2;                    // Convierte microsegundos a ticks porque en Timer1 cada tick vale 0.5 us
	OCR1A = tk;                     // Carga ese valor al registro que controla el ancho del pulso en D9
}

void PWM1_servo_set_from_adc(uint16_t adc_value) // Esta rutina convierte una lectura ADC en pulso para el servo 1
{
	uint16_t ps = 0;                // Aquí guardaremos el pulso final en microsegundos

	ps = 1000 + ((uint32_t)adc_value * 1000UL) / 1023UL; // Mapea el rango 0-1023 del ADC a 1000-2000 us
	PWM1_servo_set_pulse_us(ps);    // Manda ese pulso ya calculado al servo 1
}