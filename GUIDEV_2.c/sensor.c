#include <msp430.h>
#include <stdint.h>
#include "sensor.h"
uint8_t values[7];
//Define sensor address
const uint8_t temp[] = {0x01,0x03, 0x00, 0x01, 0x00, 0x01, 0xd5, 0xca};
const uint8_t humi[]  = {0x01,0x03,0x00,0x00,0x00,0x01,0x84,0x0a};
const uint8_t ec[] = {0x01,0x03, 0x00, 0x02, 0x00, 0x01, 0x25, 0xca};
const uint8_t ph[] = {0x01,0x03, 0x00, 0x03, 0x00, 0x01, 0x74, 0x0A};//0x0B64

const uint8_t nitro[] = { 0x01, 0x03, 0x00, 0x04, 0x00, 0x01, 0xc5, 0xcb };
const uint8_t phos[] = { 0x01, 0x03, 0x00, 0x05, 0x00, 0x01, 0x94, 0x0b };
const uint8_t pota[] = { 0x01, 0x03, 0x00, 0x06, 0x00, 0x01, 0x64, 0x0b };
// end define sensor address
int nitrogen(void)
{
    // Set DE, RE to output
    P2DIR |= BIT4 | BIT5;
    delay_ms(10);

    uint8_t i = 0;
    uint32_t timeout;     
    // TX mode
    P2OUT |= (BIT4);
    P2OUT |= BIT5;
    delay_ms(100);
    UART_SendHexArray(nitro, sizeof(nitro));
    delay_ms(5);
    //UART_SendByte(0x00);
    //while (!(UCA0IFG & UCTXIFG));

    // RX mode
    P2OUT &= ~(BIT4);
    P2OUT &= ~BIT5;    
    while (i < 7) {
        timeout = 9999;  // Reset timeout per byte
        while (!uart_available() && timeout > 0) {
            timeout--;
        }

        if (uart_available()) {
            values[i++] = uart_read();
        } else {
            values[i++] = 0xFF;  // Timeout, insert error value
        }
    }
    return (int)(((uint16_t)values[3] << 8) | values[4]);
}
//////////////////////////////////////////////////////////
int phosphorous(void)
{
    // Set DE, RE to output
    P2DIR |= BIT4 | BIT5;
    delay_ms(10);

    uint8_t i = 0;
    uint32_t timeout;     
    // TX mode
    P2OUT |= (BIT4);
    P2OUT |= BIT5;
    delay_ms(100);
    UART_SendHexArray(phos, sizeof(phos));
    delay_ms(5);
    //UART_SendByte(0x00);
    //while (!(UCA0IFG & UCTXIFG));

    // RX mode
    P2OUT &= ~(BIT4);
    P2OUT &= ~BIT5;    
    while (i < 7) {
        timeout = 9999;  // Reset timeout per byte
            while (!uart_available() && timeout > 0) {
            timeout--;
        }

        if (uart_available()) {
            values[i++] = uart_read();
        } else {
            values[i++] = 0xFF;  // Timeout, insert error value
        }
    }
    return (int)(((uint16_t)values[3] << 8) | values[4]);
}
//////////////////////////////////////////////////////////
int potassium(void)
{
    // Set DE, RE to output
    // Set DE, RE to output
    P2DIR |= BIT4 | BIT5;
    delay_ms(10);

    uint8_t i = 0;
    uint32_t timeout;     
    // TX mode
    P2OUT |= (BIT4);
    P2OUT |= BIT5;
    delay_ms(100);
    UART_SendHexArray(pota, sizeof(pota));
    delay_ms(5);
    //UART_SendByte(0x00);
    //while (!(UCA0IFG & UCTXIFG));

    // RX mode
    P2OUT &= ~(BIT4);
    P2OUT &= ~BIT5;    
    while (i < 7) {
        timeout = 9999;  // Reset timeout per byte
        while (!uart_available() && timeout > 0) {
            timeout--;
        }

        if (uart_available()) {
            values[i++] = uart_read();
        } else {
            values[i++] = 0xFF;  // Timeout, insert error value
        }
    }
    return (int)(((uint16_t)values[3] << 8) | values[4]);
}
//////////////////////////////////////////////////////////
int humidity(void)
{
    // Set DE, RE to output
    P2DIR |= BIT4 | BIT5;
    delay_ms(10);

    uint8_t i = 0;
    uint32_t timeout;     
    // TX mode
    P2OUT |= (BIT4);
    P2OUT |= BIT5;
    delay_ms(100);
    UART_SendHexArray(humi, sizeof(humi));
    delay_ms(5);
    //UART_SendByte(0x00);
    //while (!(UCA0IFG & UCTXIFG));

    // RX mode
    P2OUT &= ~(BIT4);
    P2OUT &= ~BIT5;    
    while (i < 7) {
        timeout = 9999;  // Reset timeout per byte
        while (!uart_available() && timeout > 0) {
            timeout--;
        }

        if (uart_available()) {
            values[i++] = uart_read();
        } else {
            values[i++] = 0xFF;  // Timeout, insert error value
        }
    }
    return (int)(((uint16_t)values[3] << 8) | values[4]);
}
//////////////////////////////////////////////////////////
int PH(void)
{
    // Set DE, RE to output
    P2DIR |= BIT4 | BIT5;
    delay_ms(10);

    uint8_t i = 0;
    uint32_t timeout;     
    // TX mode
    P2OUT |= (BIT4);
    P2OUT |= BIT5;
    delay_ms(100);
    UART_SendHexArray(ph, sizeof(ph));
    delay_ms(5);
    //UART_SendByte(0x00);
    //while (!(UCA0IFG & UCTXIFG));

    // RX mode
    P2OUT &= ~(BIT4);
    P2OUT &= ~BIT5;    
    while (i < 7) {
        timeout = 9999;  // Reset timeout per byte
        while (!uart_available() && timeout > 0) {
            timeout--;
        }

        if (uart_available()) {
            values[i++] = uart_read();
        } else {
            values[i++] = 0xFF;  // Timeout, insert error value
        }
    }
    return (int)(((uint16_t)values[3] << 8) | values[4]);
}
//////////////////////////////////////////////////////////
int EC(void)
{
    // Set DE, RE to output
    P2DIR |= BIT4 | BIT5;
    delay_ms(10);

    uint8_t i = 0;
    uint32_t timeout;     
    // TX mode
    P2OUT |= (BIT4);
    P2OUT |= BIT5;
    delay_ms(100);
    UART_SendHexArray(ec, sizeof(ec));
    delay_ms(5);
    //UART_SendByte(0x00);
    //while (!(UCA0IFG & UCTXIFG));

    // RX mode
    P2OUT &= ~(BIT4);
    P2OUT &= ~BIT5;    
    while (i < 7) {
        timeout = 9999;  // Reset timeout per byte
        while (!uart_available() && timeout > 0) {
            timeout--;
        }

        if (uart_available()) {
            values[i++] = uart_read();
        } else {
            values[i++] = 0xFF;  // Timeout, insert error value
        }
    }
    return (int)(((uint16_t)values[3] << 8) | values[4]);
}
//////////////////////////////////////////////////////////
int Temp(void)
{
    // Set DE, RE to output
    P2DIR |= BIT4 | BIT5;
    delay_ms(10);

    uint8_t i = 0;
    uint32_t timeout;     
    // TX mode
    P2OUT |= (BIT4);
    P2OUT |= BIT5;
    delay_ms(100);
    UART_SendHexArray(temp, sizeof(temp));
    delay_ms(5);
    //UART_SendByte(0x00);
    //while (!(UCA0IFG & UCTXIFG));

    // RX mode
    P2OUT &= ~(BIT4);
    P2OUT &= ~BIT5;    
    while (i < 7) {
        timeout = 9999;  // Reset timeout per byte
        while (!uart_available() && timeout > 0) {
            timeout--;
        }

        if (uart_available()) {
            values[i++] = uart_read();
        } else {
            values[i++] = 0xFF;  // Timeout, insert error value
        }
    }
    return (int)(((uint16_t)values[3] << 8) | values[4]);
}
//////////////////////////////////////////////////////////