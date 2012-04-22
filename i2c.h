#ifndef I2CH
#define I2CH

#define ACK 1
#define NACK 0

char i2c_start(unsigned int dev_id, unsigned int dev_addr, unsigned char rw_type);
char i2c_write(char data);
void i2c_stop(void);
char i2c_read(char *data,char ack_type);

#endif
