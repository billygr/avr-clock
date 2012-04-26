#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "avrclock.h"
#include "i2c.h"
#include "ds1307.h"

extern char ds1307_addr[7];

unsigned char display1,display2,display3,display4;
unsigned char ticks;
unsigned char timer_minutes,timer_seconds;
unsigned char timer_running;
unsigned char operation_mode=1;

void update_displays(void)
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
/*
        Timer2 overflows every 16.384mS
        Every 61 times the time passed is equal to 999.424mS (almost 1 sec)
        Error = 5.759424 seconds every 9999 seconds (2.7775 hours)
*/

        update_displays();
        ticks++;
        if (ticks>=61)
        {
        /*      One second has passed   */
                ticks=0;
                if (timer_running)
                {
                        if (timer_seconds>=0)
                        {
                                timer_seconds--;
                                LED_PORT ^= _BV(LED);
                        }
                        
                        if (timer_seconds==0 && timer_minutes>0)
                        {
                                timer_minutes--;
                                timer_seconds=60;
                        }

                        if (timer_minutes==0 && timer_seconds==0)
                        {
                                timer_running=0;
                        }
                        

                }
        }
}

void display_number(unsigned char number, unsigned char digit)
{
/* 
        For each display we clear his bit = 0 in order to be ON 
        For all the others we set their bit = 1 in order to be OFF
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

void display_decimal_value(unsigned int number)
{
        display1=(number / 1000) % 10;
        display2=(number / 100) % 10;
        display3=(number / 10) % 10;
        display4=number % 10;
}

unsigned char button1_is_pressed()
{
        /* the button is pressed when BUTTON_BIT is clear */
        if (bit_is_clear(BUTTON1_PIN, BUTTON1_BIT))
        {
        /*      Debounce wait 25 ms     */
                _delay_ms(25);
                if (bit_is_clear(BUTTON1_PIN, BUTTON1_BIT)) return 1;
        }

        return 0;
}

unsigned char button2_is_pressed()
{
        /* the button is pressed when BUTTON_BIT is clear */
        if (bit_is_clear(BUTTON2_PIN, BUTTON2_BIT))
        {
        /*      Debounce wait 25 ms     */
                _delay_ms(25);
                if (bit_is_clear(BUTTON2_PIN, BUTTON2_BIT)) return 1;
        }

        return 0;
}

unsigned char button3_is_pressed()
{
        /* the button is pressed when BUTTON_BIT is clear */
        if (bit_is_clear(BUTTON3_PIN, BUTTON3_BIT))
        {
        /*      Debounce wait 25 ms     */
                _delay_ms(25);
                if (bit_is_clear(BUTTON3_PIN, BUTTON3_BIT)) return 1;
        }

        return 0;
}
int main( void )
{
        /* Make all pins of port B/D as output	*/
        DDRB =0xFF;
        DDRD =0xFF;
        DDRC =0xFF;

        /* All displays are off */
        sbi(PORTC,DIG1);
        sbi(PORTC,DIG2);
        sbi(PORTC,DIG3);
        sbi(PORTC,DIG4);
 
        //Init Timer2 for updating the display via interrupts
        TCCR2B = (1<<CS22)|(1<<CS21)|(1<<CS20); //Set prescalar to clk/1024 : 1 click = 64us (assume 16MHz)
        TIMSK2 = (1<<TOIE2);
        //TCNT2 should overflow every 16.384 ms (256 * 64us)

        /* Initialise I2C communication */
        i2c_init();

        sei(); //Enable interrupts

        // Self test display 8888 in the displays
        sbi(PORTB, LED);
        display_hex(0x88,0x88);
        _delay_ms(500);

        /* turn on internal pull-up resistor for the switches */
        BUTTON1_PORT |= _BV(BUTTON1_BIT);
        BUTTON2_PORT |= _BV(BUTTON2_BIT);
        BUTTON3_PORT |= _BV(BUTTON3_BIT);

        while (1)
        {

                /* Cycle through modes  */
                if (button3_is_pressed())
                {
                        operation_mode++;if (operation_mode>5) operation_mode=1;
                }
                
                /* Display time */
                if (operation_mode==1)
                {
                        Read_DS1307();
                        sbi(PORTB,LED);
                        display_time(ds1307_addr[2],ds1307_addr[1]);
                }

                /* Display date */
                if (operation_mode==2)
                {
                        Read_DS1307();
                        cbi(PORTB,LED);
                        display_time(ds1307_addr[4],ds1307_addr[5]);
                }

                /* Timer setup mode   */
                if (operation_mode==3)
                {
                        cbi(PORTB,LED);
                        if (button1_is_pressed())
                        {
                                timer_minutes++;
                                if (timer_minutes>59) timer_minutes=0;
                        }
        
                        if (button2_is_pressed())
                        {
                                timer_seconds++;
                                if (timer_seconds>59) timer_seconds=0;
                        }
                        display_time(timer_minutes,timer_seconds);
                }

                /* Timer set ready to run */ 
                if (operation_mode==4)
                {
                        if (timer_running==0) sbi(PORTB,LED);
                        if (button1_is_pressed()) {timer_running=1;cbi(PORTB,LED);};
                        if (button2_is_pressed()) timer_running=0;
                        if (button3_is_pressed()) {timer_running=0;operation_mode=1;}
                        display_time(timer_minutes,timer_seconds);
                }
        }
}
