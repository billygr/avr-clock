#ifndef DS1307H
#define DS1307H

#define DS1307_ID    0xD0        // I2C DS1307 Device Identifier
#define DS1307_ADDR  0x00        // I2C DS1307 Device Address

char bcd2dec(char num);
char dec2bcd(char num);

void Read_DS1307(void);
void Write_DS1307(void);

#endif
