#include <msp430.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "LCD.h"
#define lcd_RS  BIT0
#define lcd_RW BIT1
#define lcd_EN  BIT2
uint8_t lcd_Command_mode = 0;
uint8_t lcd_Data_mode = 1;
// #define lcd_port PORTD
#define lcd_port  P9OUT
unsigned int lcd_cursor(unsigned int row, unsigned int col)
{
    unsigned int cursor[2][16] = { {0X00, 0X01, 0X02, 0X03, 0X04, 0X05, 0X06, 0X07, 0X08, 0X09, 0X0A, 0X0B, 0X0C, 0X0D, 0X0E, 0X0F },
    {0X40, 0X41, 0X42, 0X43, 0X44, 0X45, 0X46, 0X47, 0X48, 0X49, 0X4A, 0X4B, 0X4C, 0X4D, 0X4E, 0X4F }
   
}; 
    return cursor[col][row];
    
}
void lcd_write_4bit(uint8_t mode, unsigned char data) // 4-bit data mode
{	lcd_port = data; 
	if(mode == 0)
{
	P9OUT &= ~lcd_RS;
}
	else if(mode == 1)
{
	P9OUT |= lcd_RS;
}
	P9OUT &=  ~lcd_RW;  //lcd_RW = 0
	P9OUT |= lcd_EN; //lcd_EN = 1; // Enable LCD.
	delay_ms(2);
	P9OUT &=  ~lcd_EN; //lcd_EN =  0; // Disable LCD.
	delay_ms(2);
}


void lcd_write(uint8_t mode, unsigned char data)
{ 	// 4-bit data mode
	lcd_write_4bit(mode, data & 0xF0);
	lcd_write_4bit(mode, (data<<4) & 0xF0);
}
void lcd_init()
{
	delay_ms(40);
	lcd_write_4bit(lcd_Command_mode, 0x30); // Set as 4 bit data mode
	delay_ms(5); // Delay 5 ms
	lcd_write_4bit(lcd_Command_mode, 0x30); // Set as 4 bit data mode
	delay_us(200); // Delay 200 us
	lcd_write_4bit(lcd_Command_mode, 0x30); // Set as 4 bit data mode
	lcd_write_4bit(lcd_Command_mode, 0x20); // Set as 4 bit data mode
	lcd_write(lcd_Command_mode, 0x28); 
	lcd_write(lcd_Command_mode, 0x08);
	lcd_write(lcd_Command_mode, 0x01); // Display Clear 
	lcd_write(lcd_Command_mode, 0x06);
}

int lcd_write_string(unsigned char position, char *ptr)
{
	int i = 0;
	lcd_write(lcd_Command_mode, 0x80 | position ); 
	while ( *ptr != 0)
   	{       
		lcd_write(lcd_Data_mode, *ptr++);
		delay_ms(2);
		i++;
   	}
	return i;
}

void lcd_clear()
{
	lcd_write(lcd_Command_mode, 0x01); // Display Clear 
}
