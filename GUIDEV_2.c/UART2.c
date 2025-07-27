#include <msp430.h>
#include <stdint.h>
#include "UART2.h"
#include <stdio.h>
#include <stdlib.h>
volatile char uart_buffer[100];
volatile unsigned int buffer_index = 0;
volatile unsigned char recording = 0;  // flag to indicate if we are currently recording
char *token;
char *zero = "0";  // Reusable pointer to "0" string
char lat_deg[3];  // 2 digits + '\0'
char lat_min[12];

char lon_deg[4];  // 3 digits + '\0'
char lon_min[12];
#define MAX_FIELDS 20
#define MAX_FIELD_LEN 20
void UART2_Init(void)
{
    // Configure UART1 pins (P3.4 -> UCA0TXD, P3.5 -> UCA0RXD)
    P3SEL0 |=(BIT4 | BIT5); // USCI_A0 UART operation
    P3SEL1 &= ~(BIT5 | BIT5);
    
    UCA1CTLW0 = UCSWRST;                  // Put eUSCI in reset
    UCA1CTLW0 |= UCSSEL_3;           // Select SMCLK (16MHz)
    
    // Baud Rate Configuration for 9600 bps with 16MHz clock
    UCA1BRW = 6;
    //UCBRF = 8, UCBRS = 0X20
    UCA1MCTLW = UCBRF3 | UCBRS5 | UCOS16;
    
    UCA1CTLW0 &= ~UCSWRST;                // Initialize eUSCI EXIT RESET
    UCA1IE &= ~UCRXIE;                     // Disable RX interrupt (optional)
}

void UART2_SendChar(uint8_t c)
{
    while (!(UCA1IFG & UCTXIFG)); // Wait until TX buffer is ready
    UCA1TXBUF = c;
}

uint8_t UART2_ReceiveChar(void)
{
    while (!(UCA1IFG & UCRXIFG)); // Wait for RX buffer to be filled
    return UCA1RXBUF;
}

void UART2_SendString(const char *str)
{
    while (*str)
    {
        UART2_SendChar(*str++);
    }
}
void UART2_SendStringArray(const char *arr)
{
    while (*arr)
    {
        UART2_SendChar(*arr++);
    }
}
void UART2_SendHexArray(const uint8_t *arr, uint8_t length)
{
    int i;
    for (i = 0; i < length; i++)
    {
        UART2_SendByte(arr[i]);
    }
    //UART2_SendString("\n");
}
void UART2_SendByte(uint8_t data)
{
    while (!(UCA1IFG & UCTXIFG)); // Wait for TX buffer to be empty
    UCA1TXBUF = data;
}
// Helper to convert int to char and send
void UART2_SendInt(long num) {
    char buf[12];  // Enough for 32-bit numbers
    int i = 0;

    if (num == 0) {
        UART2_SendChar('0');
        return;
    }

    if (num < 0) {
        UART2_SendChar('-');
        num = -num;
    }

    while (num > 0) {
        buf[i++] = (num % 10) + '0';
        num /= 10;
    }

    while (i--) {
        UART2_SendChar(buf[i]);
    }
}

void UART2_SendFloat(float val) {
    long scaled = (long)(val * 100);  // Use long to avoid 16-bit overflow

    long int_part = scaled / 100;
    long decimal_part = scaled % 100;

    UART2_SendInt(int_part);          // Send integer part
    UART2_SendChar('.');             // Decimal point

    // Pad with '0' if needed for decimals like ".07"
    if (decimal_part < 10)
        UART2_SendChar('0');
    UART2_SendInt(decimal_part);     // Send decimal part
}

// Sends a float over UART as ASCII (e.g., 12.34 → '1' '2' '.' '3' '4')
void floatToString(float value, char* buffer, int precision) {
    // Handle zero explicitly
    int i;
    if (value == 0.0f) {
        *buffer++ = '0';
        if (precision > 0) {
            *buffer++ = '.';
            for ( i = 0; i < precision; i++) {
                *buffer++ = '0';
            }
        }
        *buffer = '\0';
        return;
    }

    // Handle negative numbers
    if (value < 0) {
        *buffer++ = '-';
        value = -value;
    }

    // Integer part
    int intPart = (int)value;
    float frac = value - (float)intPart;

    // Convert integer part
    char temp[10];
    int j;
    do {
        temp[i++] = (intPart % 10) + '0';
        intPart /= 10;
    } while (intPart && i < sizeof(temp));

    // Reverse integer part into buffer
    while (i--)
        *buffer++ = temp[i];

    // Decimal point
    if (precision > 0) {
        *buffer++ = '.';

        // Convert fractional part
        for ( j = 0; j < precision; j++) {
            frac *= 10;
            int digit = (int)frac;
            *buffer++ = digit + '0';
            frac -= digit;
        }
    }

    *buffer = '\0';  // Null-terminate
}

// UART RX ISR (optional)
#pragma vector=USCI_A1_VECTOR
__interrupt void USCI_A1_ISR(void)
{
    if (UCA1IFG & UCRXIFG)
    {
        char received = UCA1RXBUF; // Read received data
        if(received == '$')
        {
            buffer_index = 0;
            recording = 1;
            uart_buffer[buffer_index] = received;
        }
        if(recording == 1)
        {
            if(received != '\r' || received != '\n')
            {
                buffer_index += 1;
                uart_buffer[buffer_index] = received;
            }
            if(received == '\r' || received == '\n')
            {
                buffer_index = 0;
                recording = 0;
                split();
            }
        }

    }
}
// char *getTokenOrZero() {
//     token = strtok(NULL, ",");
//     if (token == NULL || strlen(token) == 0) return zero;
//     return token;
// }
void splitLatLong(const char *input, int degreeDigits, char *degPart, char *minPart) {
    // Copy degree part
    strncpy(degPart, input, degreeDigits);
    degPart[degreeDigits] = '\0';

    // Copy minute part
    strcpy(minPart, input + degreeDigits);
}


void splitNMEASentence(const char *sentence, char fields[MAX_FIELDS][MAX_FIELD_LEN]) {
    int field = 0, charIdx = 0,i;

    for ( i = 0; sentence[i] != '\0' && field < MAX_FIELDS; i++) {
        if (sentence[i] == ',') {
            fields[field][charIdx] = '\0';  // Terminate current field
            field++;
            charIdx = 0;
        } else {
            if (charIdx < MAX_FIELD_LEN - 1) {
                fields[field][charIdx++] = sentence[i];
            }
        }
    }
    fields[field][charIdx] = '\0';  // Final field
}
void split()
{
    char fields[MAX_FIELDS][MAX_FIELD_LEN];

    splitNMEASentence(uart_buffer, fields);

char *lat         = fields[2][0] ? fields[2] : "0";
char *longi       = fields[4][0] ? fields[4] : "0";



    splitLatLong(lat, 2, lat_deg, lat_min);
    splitLatLong(longi, 3, lon_deg, lon_min);

    // UART2_SendString("lat ");
    // UART2_SendString(lat);
    // UART2_SendString("\r\n");

    // UART2_SendString("longi ");
    // UART2_SendString(longi);
    // UART2_SendString("\r\n");

    // UART2_SendString("lat_deg ");
    // UART2_SendString(lat_deg);
    // UART2_SendString("\r\n");

    // UART2_SendString("lat_min ");
    // UART2_SendString(lat_min);
    // UART2_SendString("\r\n");

    // UART2_SendString("lon_deg ");
    // UART2_SendString(lon_deg);
    // UART2_SendString("\r\n");

    // UART2_SendString("lon_min ");
    // UART2_SendString(lon_min);
    // UART2_SendString("\r\n");

}