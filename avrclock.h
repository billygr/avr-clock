#ifndef AVRCLOCKH
#define AVRCLOCKH

/* Unused pins
PB4 18 MISO
PB5 19 SCK
*/

#define sbi(port, pin)   ((port) |= (uint8_t)(1 << pin))
#define cbi(port, pin)   ((port) &= (uint8_t)~(1 << pin))

/* Connections to the PCB       */
#define DISPLAY_PORT PORTD

#define LED_PORT PORTB
#define LED PORTB0

#define BUTTON1_PORT PORTB
#define BUTTON1_PIN PINB
#define BUTTON1_BIT PORTB1

#define BUTTON2_PORT PORTB
#define BUTTON2_PIN PINB
#define BUTTON2_BIT PORTB2

#define MODEBUTTON_PORT PORTB
#define MODEBUTTON_PIN PINB
#define MODEBUTTON_BIT PORTB3

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

/*
        Lookup table for the seven segment displays
        Uggly but it works
*/

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

void update_displays(void);
unsigned char button_is_pressed(unsigned char BUTTON_PINX, unsigned char BUTTON_BITX);
void display_number(unsigned char number, unsigned char digit);
void display_time(unsigned char hours, unsigned char minutes);
void display_hex(unsigned char number1, unsigned char number2);
void display_decimal_value(unsigned int number);

#endif
