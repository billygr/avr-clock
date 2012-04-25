#ifndef AVRCLOCKH
#define AVRCLOCKH

/* Unused pins
PB1 15
PB2 16
PB3 17 MOSI
PB4 18 MISO
PB5 19 SCK
*/

void update_displays(void);
void display_number(unsigned char number, unsigned char digit);
void display_time(unsigned char hours, unsigned char minutes);
void display_hex(unsigned char number1, unsigned char number2);
void display_decimal_value(unsigned int number);

#endif
