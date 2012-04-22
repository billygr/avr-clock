#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "i2c.h"
#include "ds1307.h"

extern char ds1307_addr[7];

#define sbi(port, pin)   ((port) |= (uint8_t)(1 << pin))
#define cbi(port, pin)   ((port) &= (uint8_t)~(1 << pin))


/* Connections to the PCB       */
#define LED PORTB0
#define D PD0
#define E PD1
#define C PD2
#define DP PD3
#define B PD4
#define A PD5
#define G PD6
#define F PD7

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
        PORTD=0;
}

ISR (SIG_OVERFLOW2)
{
        update_displays();
}

void display_number(unsigned char number, unsigned char digit)
{

        switch (digit) {
        case 0: PORTC=254;break;
        case 1: PORTC=253;break;
        case 2: PORTC=251;break;
        case 3: PORTC=247;break;
        }

        PORTD = seven_segment_lookup[number];
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

//ds1307_addr[0]=0;
//ds1307_addr[1]=41;
//ds1307_addr[2]=8;
//ds1307_addr[3]=1;
//ds1307_addr[4]=22;
//ds1307_addr[5]=4;
//ds1307_addr[6]=12;
//Write_DS1307();

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

        sbi(PORTB, LED);
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
