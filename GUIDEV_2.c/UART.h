#include <msp430.h>
#include <stdint.h>
void UART_Init(void);
void UART_SendChar(uint8_t c);
uint8_t UART_ReceiveChar(void);
void UART_SendString(const char *str);
void UART_SendStringArray(const char *arr, uint8_t length);
void UART_SendHexArray(const uint8_t *arr, uint8_t length);
void UART_SendByte(uint8_t data);
void UART_GetSensorData(void);
void byte_to_hex(unsigned char byte, char *out);
int uart_available();
uint8_t uart_read();