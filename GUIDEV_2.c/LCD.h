#include <msp430.h>
#include <stdint.h>
extern uint8_t lcd_Command_mode;
extern uint8_t lcd_Data_mode;
unsigned int lcd_cursor(unsigned int row, unsigned int col);
void lcd_write_4bit(uint8_t mode, unsigned char data); // 4-bit data mode
void lcd_write(uint8_t mode, unsigned char data);
void lcd_init();
int lcd_write_string(unsigned char position, char *ptr);
void lcd_clear();
