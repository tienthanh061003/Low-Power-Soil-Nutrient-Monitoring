#include <msp430.h>
#include <stdint.h>
#include "UART.h"
#include <stdio.h>
#include <stdlib.h>
#define RX_BUF_SIZE 64
volatile uint8_t rx_buffer[RX_BUF_SIZE];
volatile uint8_t rx_head = 0;
volatile uint8_t rx_tail = 0;
int uart_available()
{
    return rx_head != rx_tail;
}

uint8_t uart_read()
{
    uint8_t data = rx_buffer[rx_tail];
    rx_tail = (rx_tail + 1) % RX_BUF_SIZE;
    return data;
}
void UART_Init(void)
{
    // Configure UART1 pins (P3.4 -> UCA0TXD, P3.5 -> UCA0RXD)
    P2SEL0 |=(BIT0 | BIT1); // USCI_A0 UART operation
    P2SEL1 &= ~(BIT0 | BIT1);
    
    UCA0CTLW0 = UCSWRST;                  // Put eUSCI in reset
    UCA0CTLW0 |= UCSSEL_3;           // Select SMCLK (16MHz)
    
    // Baud Rate Configuration for 9600 bps with 16MHz clock
    UCA0BRW = 6;
    //UCBRF = 8, UCBRS = 0X20
    UCA0MCTLW = UCBRF3 | UCBRS5 | UCOS16;
    
    UCA0CTLW0 &= ~UCSWRST;                // Initialize eUSCI EXIT RESET
    UCA0IE |= UCRXIE;                     // Disable RX interrupt (optional)
}

void UART_SendChar(uint8_t c)
{
    while (!(UCA0IFG & UCTXIFG)); // Wait until TX buffer is ready
    UCA0TXBUF = c;
}

uint8_t UART_ReceiveChar(void)
{
    while (!(UCA0IFG & UCRXIFG)); // Wait for RX buffer to be filled
    return UCA0RXBUF;
}

void UART_SendString(const char *str)
{
    while (*str)
    {
        UART_SendChar(*str++);
    }
}
void UART_SendStringArray(const char *arr, uint8_t length)
{
    int i;
    for (i = 0; i < length; i++)
    {
        UART_SendChar(arr[i]);
    }
    //UART_SendString("\n");
}
void UART_SendHexArray(const uint8_t *arr, uint8_t length)
{
    int i;
    for (i = 0; i < length; i++)
    {
        UART_SendByte(arr[i]);
    }
    //UART_SendString("\n");
}
void UART_SendByte(uint8_t data)
{
    while (!(UCA0IFG & UCTXIFG)); // Wait for TX buffer to be empty
    UCA0TXBUF = data;
}

void UART_GetSensorData(void)
{
    uint32_t timeout = 9000; // adjust depending on your clock and baud rate
    while (!(UCA0IFG & UCRXIFG))
    {
            if (--timeout == 0)
            return 0xFF; // or a custom error value 
    } // Wait for RX buffer to be filled
    return UCA0RXBUF;
}

// UART RX ISR (optional)

void byte_to_hex(unsigned char byte, char *out)
{
    const char hex[] = "0123456789ABCDEF";
    out[0] = hex[(byte >> 4) & 0x0F];
    out[1] = hex[byte & 0x0F];
}
#pragma vector=USCI_A0_VECTOR
__interrupt void USCI_A0_ISR(void)
{
    if (UCA0IFG & UCRXIFG)
    {
        //char received = UCA0RXBUF; // Read received data
        rx_buffer[rx_head] = UCA0RXBUF;
        rx_head = (rx_head + 1) % RX_BUF_SIZE;
    }
}