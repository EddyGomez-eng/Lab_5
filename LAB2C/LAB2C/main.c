#define F_CPU 16000000UL             // Le dice al compilador que el micro trabaja a 16 MHz

#include <avr/io.h>                  // Trae los registros y puertos del AVR
#include <avr/interrupt.h>           // Permite habilitar interrupciones globales
#include <util/delay.h>              // Permite usar retardos como _delay_ms
#include <stdint.h>                  // Permite usar tipos enteros de tamaño fijo
#include "pwm1_servo.h"              // Incluye la librería propia del servo 1
#include "pwm2_servo.h"              // Incluye la librería propia del servo 2
#include "pwm3_led.h"                // Incluye la librería propia del LED con PWM manual

void ADC_init(void);                 // Declara la rutina que configura el ADC
uint16_t ADC_read(uint8_t canal);    // Declara la rutina que lee un canal analógico
uint16_t ADC_read_avg(uint8_t canal); // Declara la rutina que promedia varias lecturas del ADC
uint16_t abs_diff(uint16_t a, uint16_t b); // Declara la rutina que calcula diferencia absoluta

int main(void)                       // Aquí comienza el programa principal
{
	uint16_t vl0 = 0;                // Guardará la lectura filtrada del potenciómetro conectado a A0
	uint16_t vl1 = 0;                // Guardará la lectura filtrada del potenciómetro conectado a A1
	uint16_t vl2 = 0;                // Guardará la lectura filtrada del potenciómetro conectado a A2

	uint16_t an0 = 0;                // Guarda el valor anterior aceptado para el servo 1
	uint16_t an1 = 0;                // Guarda el valor anterior aceptado para el servo 2
	uint16_t an2 = 0;                // Guarda el valor anterior aceptado para el LED

	uint16_t fl2 = 0;                // Guarda un valor suavizado extra para el LED
	uint8_t cd2 = 0;                 // Este contador hará que el LED se actualice un poco más lento

	ADC_init();                      // Primero deja configurado el ADC
	PWM1_servo_init();               // Después deja listo el PWM1 para el servo 1
	PWM2_servo_init();               // Luego deja listo el PWM2 para el servo 2
	PWM3_led_init();                 // Después deja listo el PWM manual para el LED

	sei();                           // Habilita las interrupciones globales para que el Timer0 trabaje

	an0 = ADC_read_avg(0);           // Toma una lectura inicial estable para el servo 1
	an1 = ADC_read_avg(1);           // Toma una lectura inicial estable para el servo 2
	fl2 = ADC_read_avg(2);           // Toma una lectura inicial estable para el LED
	an2 = fl2;                       // Hace que el valor anterior del LED arranque desde el filtrado inicial

	PWM1_servo_set_from_adc(an0);    // Coloca el servo 1 en una posición inicial estable
	PWM2_servo_set_from_adc(an1);    // Coloca el servo 2 en una posición inicial estable
	PWM3_led_set_from_adc(an2);      // Coloca el LED en un brillo inicial estable

	while (1)                        // Entra en un ciclo infinito para que el sistema nunca se detenga
	{
		vl0 = ADC_read_avg(0);       // Lee A0 varias veces y promedia para reducir ruido
		vl1 = ADC_read_avg(1);       // Lee A1 varias veces y promedia para reducir ruido
		vl2 = ADC_read_avg(2);       // Lee A2 varias veces y promedia para reducir ruido

		if (abs_diff(vl0, an0) > 10) // Solo actualiza el servo 1 si el cambio ya es suficientemente grande
		{
			an0 = vl0;               // Guarda el nuevo valor aceptado del servo 1
			PWM1_servo_set_from_adc(an0); // Actualiza la posición del servo 1
		}

		if (abs_diff(vl1, an1) > 10) // Solo actualiza el servo 2 si el cambio ya es suficientemente grande
		{
			an1 = vl1;               // Guarda el nuevo valor aceptado del servo 2
			PWM2_servo_set_from_adc(an1); // Actualiza la posición del servo 2
		}

		cd2++;                       // Aumenta el contador que controla cada cuánto se refresca el LED

		if (cd2 >= 4)                // Hace que el LED se actualice más seguido para responder más rápido
		{
			cd2 = 0;                 // Reinicia el contador para comenzar otra espera

			fl2 = (fl2 * 7 + vl2) / 8; // Suaviza el valor del LED, pero con menos filtro para que reaccione antes

			if (abs_diff(fl2, an2) > 8) // Solo actualiza el LED si el cambio ya es suficientemente apreciable
			{
				an2 = fl2;           // Guarda el nuevo valor aceptado del LED
				PWM3_led_set_from_adc(an2); // Actualiza el brillo del LED
			}
		}

		_delay_ms(10);               // Da una pequeña pausa para no refrescar exageradamente rápido
	}
}

void ADC_init(void)                  // Esta rutina deja listo el módulo ADC
{
	ADMUX = 0;                       // Limpia la configuración previa del multiplexor ADC
	ADCSRA = 0;                      // Limpia la configuración previa del control ADC

	ADMUX |= (1 << REFS0);           // Selecciona AVcc como referencia del ADC

	ADCSRA |= (1 << ADEN);           // Enciende el ADC para permitir conversiones
	ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // Coloca prescaler 128 para un rango seguro de trabajo
}

uint16_t ADC_read(uint8_t canal)     // Esta rutina lee un canal analógico y devuelve su valor
{
	ADMUX &= 0xF0;                   // Limpia solo los bits del canal sin tocar la referencia
	ADMUX |= (canal & 0x0F);         // Coloca el canal que queremos leer

	ADCSRA |= (1 << ADSC);           // Inicia la conversión analógica digital

	while (ADCSRA & (1 << ADSC));    // Espera hasta que la conversión termine

	return ADC;                      // Devuelve el valor final de 10 bits
}

uint16_t ADC_read_avg(uint8_t canal) // Esta rutina lee varias veces el ADC y devuelve un promedio
{
	uint32_t sm = 0;                 // Aquí se acumulan las lecturas para luego promediarlas

	sm += ADC_read(canal);           // Primera lectura del canal
	sm += ADC_read(canal);           // Segunda lectura del canal
	sm += ADC_read(canal);           // Tercera lectura del canal
	sm += ADC_read(canal);           // Cuarta lectura del canal

	return (uint16_t)(sm / 4);       // Devuelve el promedio de las 4 lecturas
}

uint16_t abs_diff(uint16_t a, uint16_t b) // Esta rutina calcula la diferencia absoluta entre dos valores
{
	if (a > b)                       // Si el primer valor es mayor
	{
		return a - b;                // Devuelve la resta en ese orden
	}
	else                             // Si el segundo valor es mayor o igual
	{
		return b - a;                // Devuelve la resta en el orden contrario
	}
}