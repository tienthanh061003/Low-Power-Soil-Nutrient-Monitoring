#include <msp430.h>
#include <stdint.h>
void UART2_Init(void);
void UART2_SendChar(uint8_t c);
uint8_t UART2_ReceiveChar(void);
void UART2_SendString(const char *str);
void UART2_SendStringArray(const char *arr);
void UART2_SendHexArray(const uint8_t *arr, uint8_t length);
void UART2_SendByte(uint8_t data);
void floatToString(float value, char* buffer, int precision);
// Helper to convert int to char and send
void UART2_SendInt(long num);
void UART2_SendFloat(float val);