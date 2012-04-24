#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "avrclock.h"
#include "i2c.h"
#include "ds1307.h"

extern char ds1307_addr[7];

#define sbi(port, pin)   ((port) |= (uint8_t)(1 << pin))
#define cbi(port, pin)   ((port) &= (uint8_t)~(1 << pin))


/* Connections to the PCB       */
#define DISPLAY_PORT PORTD
#define LED PORTB0
#define D PORTD0
#define E PORTD1
#define C PORTD2
#define DP PORTD3
#define B PORTD4
#define A PORTD5
#define G PORTD6
#define F PORTD7

#define DIG1 PORTC0
#define DIG2 PORTC1
#define DIG3 PORTC2
#define DIG4 PORTC3

char seven_segment_lookup[16] = {
183, //0
20,  //1
115, //2
117, //3
212, //4
229, //5
231, //6
52,  //7
247, //8
245, //9
246, //A
199, //B
163, //C
87,  //D
227, //E
226  //F
};

unsigned char display1,display2,display3,display4;

void update_displays()
{
        display_number(display1,0);
        _delay_ms(3);
        display_number(display2,1);
        _delay_ms(3);
        display_number(display3,2);
        _delay_ms(3);
        display_number(display4,3);
        _delay_ms(3);
        DISPLAY_PORT=0;
}

ISR (SIG_OVERFLOW2)
{
        update_displays();
}

void display_number(unsigned char number, unsigned char digit)
{
/* For each display we clear his bit = Zero in order to be on
For all the others we set his bit = 1 in order to be off
*/
        switch (digit) {
        case 0: 
                {
                cbi(PORTC,DIG1);
                sbi(PORTC,DIG2);
                sbi(PORTC,DIG3);
                sbi(PORTC,DIG4);break;
                }
        case 1: 
                {
                cbi(PORTC,DIG2);
                sbi(PORTC,DIG1);
                sbi(PORTC,DIG3);
                sbi(PORTC,DIG4);break;
                }
        case 2: 
                {
                cbi(PORTC,DIG3);
                sbi(PORTC,DIG1);
                sbi(PORTC,DIG2);
                sbi(PORTC,DIG4);break;
                }

        case 3:
                {
                cbi(PORTC,DIG4);
                sbi(PORTC,DIG1);
                sbi(PORTC,DIG2);
                sbi(PORTC,DIG3);break;
                }
        }

        DISPLAY_PORT = seven_segment_lookup[number];
}

void display_time(unsigned char hours, unsigned char minutes)
{
        display1=hours / 10;
        display2=hours % 10;

        display3=minutes / 10;
        display4=minutes % 10;

}

void display_hex(unsigned char number1, unsigned char number2)
{
        display1=number1 / 16;
        display2=number1 & 0x0F;

        display3=number2 / 16;
        display4=number2 & 0x0F;
}

int main( void )
{
        //set SCL to 400kHz
        TWSR = 0x00;
        TWBR = 0x0C;
        //enable TWI
        TWCR = (1<<TWEN);

        /* Make all pins of port B/D as output	*/
        DDRB =0xFF;
        DDRD =0xFF;
        DDRC =0xFF;

        /* All displays are off */
        PORTC = 0xFF;

        //Init Timer2 for updating the display via interrupts
        TCCR2B = (1<<CS22)|(1<<CS21)|(1<<CS20); //Set prescalar to clk/1024 : 1 click = 64us (assume 16MHz)
        TIMSK2 = (1<<TOIE2);
        //TCNT2 should overflow every 16.384 ms (256 * 64us)

        sei(); //Enable interrupts

        // Self test display 8888 in the displays
        sbi(PORTB, LED);
        display_hex(0x88,0x88);
        _delay_ms(500);

        while (1){

                Read_DS1307();
                display_time(ds1307_addr[2],ds1307_addr[1]);
                _delay_ms(1000);
                cbi(PORTB,LED);
                display_time(ds1307_addr[4],ds1307_addr[5]);
                _delay_ms(1000);
                sbi(PORTB,LED);
        }
}
