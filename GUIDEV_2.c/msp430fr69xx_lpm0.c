#include <stdint.h>
#include <msp430fr5989.h>
#include <msp430.h>
#include "LCD.h"
#include "UART.h"
#include "UART2.h"
#include "delay.h"
#include "sensor.h"
#include "intrinsics.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//define LOW - HIGH for easy access
#define LOW 0
#define HIGH 1
//define prototype function
void setup();
void Show_state(uint8_t st);
void Show_lcd(uint8_t sta);
void mode0(int state);
void mode1();
void mode2();
void mode3();
void config_timer();
void Show_data(); 
int display_float(float data, uint8_t x, uint8_t y);
void display_int(int data, uint8_t x, uint8_t y);
void toLPM3(void);
void setRTCAlarmOneMinuteLater(void);
void initRTC(void);
void updateAlarm(void);
int alarm_plus = 1;
//void senddata(int var1, int var2, int var3);
//end define prototype function
volatile unsigned int overflow_count = 0;

//define gps
float lattitude = 0, longtitude = 0;
//end define gps


// define button state/ GUI state for GUI
int BTst_1 = 0;         // current state of the button 1
int BTst_2 = 0;         // current state of the button 2
int BTst_3 = 0;         // current state of the button 3

int Lt_BTst_1 = 0;     // previous state of the button 1
int Lt_BTst_2 = 0;     // previous state of the button 2
int Lt_BTst_3 = 0;     // previous state of the button 2
int mode, state, system_mode;
int minuteCounter;
volatile unsigned char next_minute;
volatile unsigned char next_hour;
// end define button state 

//define sensor value


// Define Sensor value
float val1 = 0, val2 = 0, val3 = 0, val4 = 0, val5 = 0, val6 = 0, val7 = 0, temp_data = 0;
float slope_N, slope_P, slope_K;
float itc_N, itc_P, itc_K;
float N_c, P_c, K_c; //calibrate value
// end define sensor value
extern char lat_deg[3];  // 2 digits + '\0'
extern char lat_min[12];

extern char lon_deg[4];  // 3 digits + '\0'
extern char lon_min[12];

int main(void)
{ 
    setup();
    BTst_1 = (P5IN & BIT1) ? 1 : 0;
    BTst_2 = (P5IN & BIT2) ? 1 : 0;
    BTst_3 = (P5IN & BIT3) ? 1 : 0;
  if(BTst_1 == LOW && BTst_2 == LOW)
  {
    system_mode = 0;
    
  }
  else if(BTst_2 == LOW && BTst_3 == LOW)
  {
    system_mode = 1;
    alarm_plus = 6;
      lcd_init();
      lcd_write(lcd_Command_mode, 0x0E);
      lcd_write_string(lcd_cursor(0,0), "Ultra sleep");
      delay_ms(5000);
  }
  else if (BTst_1 == LOW && BTst_3 == LOW)
  {
    system_mode = 2;
      lcd_init();
      lcd_write(lcd_Command_mode, 0x0E);
      lcd_write_string(lcd_cursor(0,0), "Unused mode");
      delay_ms(5000);
      BTst_1 = (P5IN & BIT1) ? 1 : 0;
      BTst_2 = (P5IN & BIT2) ? 1 : 0;
      BTst_3 = (P5IN & BIT3) ? 1 : 0;      
      if(BTst_1 == LOW && BTst_2 == LOW && BTst_3 == LOW)
      {
        //cheat here
        system_mode = 0;
        setup();
        val1 = 261;
        val2 = 568;
        val3 = 749;
        val4 = 46.3;
        val5 = 7.3;
        val6 = 1159;
        val7 = 28.1;
        lattitude = 10.878;
        longtitude = 106.801;

      }
  }
    else 
  {
    system_mode = 1;
      lcd_init();
      lcd_write(lcd_Command_mode, 0x0E);
      lcd_write_string(lcd_cursor(0,0), "Normal sleep");
      delay_ms(5000);
  }    

  if(system_mode == 1)
  {
    while(1)
    {
      setup();
      lcd_init();
      lcd_write(lcd_Command_mode, 0x0E);
      lcd_write_string(lcd_cursor(0,0), "Automotation state");
      mode0(1);
      mode2();
      mode3();
      initRTC();
      toLPM3();
    }
  }
  else if(system_mode == 2)
  {
    while(1)
    {
      setup();
      lcd_init();
      lcd_write(lcd_Command_mode, 0x0E);
      lcd_write_string(lcd_cursor(0,0), "Mode run");
      mode0(1);
      mode2();
      mode3();
      lcd_init();
      lcd_write(lcd_Command_mode, 0x0E);
      lcd_write_string(lcd_cursor(0,0), "End one cycle");
      
      delay_ms(50000);
      P5DIR |= BIT0;     // Set P5.0 as output
      P5OUT &= ~BIT0;    // Set P5.0 low

      //P2.3 to low level: SIM MASTER OFF, cut power to SIM7020
      P2DIR |= BIT3;     // Set P2.3 as output
      P2OUT &= ~BIT3;    // Set P2.3 low

      //P3.6 to low level: SIM PWRKEY OFF, deactivate SIM7020
      P3DIR |= BIT6;     // Set P3.6 as output
      P3OUT &= ~BIT6;    // Set P3.6 low
      delay_ms(10000);

    }
  }
  else if (system_mode == 0) 
  {
    while (1)
  { 

    //get button state
    BTst_1 = (P5IN & BIT1) ? 1 : 0;
    BTst_2 = (P5IN & BIT2) ? 1 : 0;
    BTst_3 = (P5IN & BIT3) ? 1 : 0;
    //debounce function (falling edge)

    if (BTst_1 != Lt_BTst_1) 
    {
      if(BTst_1 == LOW)
      {
        state +=1;
        
        if(state > 6)
        {
          state = 0;
        }
        Show_lcd(state);
      }

    }
    
    if(BTst_2 != Lt_BTst_2)
    {
      if(BTst_2 == LOW)
      {
        state -=1;
        if(state < 0)
        {
          state = 5;
        }
        Show_lcd(state);

      }
    }
    
   if (BTst_3 != Lt_BTst_3)
    {
      if( BTst_3 == LOW)
       { 
        Show_state(state);
        state = 0;// reset state
        Show_lcd(state); 

       }
  
    }

    Lt_BTst_3 = BTst_3; //update Last value of Button to current status
    Lt_BTst_2 = BTst_2; //update Last value of Button to current status
    Lt_BTst_1 = BTst_1; //update Last value of Button to current status
  }
  
}
}

void setup()
{
  WDTCTL = WDTPW | WDTHOLD;  // Stop watchdog timer
  P9DIR |= 0xFF; // Set p9.0 -> P9.7 to output for LCD
  PM5CTL0 &= ~LOCKLPM5;      // Unlock GPIO (needed for FRAM devices)
  // Set DCO to 16MHz
  CSCTL0_H = CSKEY_H;        // Unlock CS registers
  CSCTL1 = 0x0000;        // Set DCO to 1MHz
  CSCTL2 = SELM_3 | SELS_3 | SELA_0;
  CSCTL3 = DIVA_0 | DIVS_0 | DIVM_0; // Set all dividers to 0
  CSCTL4 &= ~LFXTOFF; //enable LFXTAL
  CSCTL0_H = 0;              // Lock CS registers
  //pin define

  //BUTTON
P5DIR &= ~(BIT1 | BIT2 | BIT3);            // Set as input
P5OUT |=  (BIT1 | BIT2 | BIT3);            // Configure as pull-up
P5REN |=  (BIT1 | BIT2 | BIT3);            // Enable pull-up resistors
  //P5.0 to high level for power acess for other port
  P5OUT |= BIT0;            // Set pull-up resistor on P5
  P5DIR |= BIT0;
  P5OUT |= BIT0;
  //P2.3 to high level, SIM MASTER ON: POWER SIM7020
  P2DIR |= BIT3;
  P2OUT |= BIT3;
 //P3.6 to high level, SIM PWRKEY ON: ACTIVATE SIM7020
  P3DIR |= BIT6;
  P3OUT |= BIT6; 
  //end define pin
  UART_Init();
  UART2_Init();
  //UART_SendString("UART Initialized\r\n");
  //UART2_SendString("UART Initialized\r\n");
  //mode0(0);
  lcd_init();
  lcd_write(lcd_Command_mode, 0x0E);
  lcd_write_string(lcd_cursor(0,0), "> Get data <");
  lcd_write_string(lcd_cursor(0,1), "Calib data");
  __enable_interrupt();
}
void Show_state(uint8_t st)
{
  
  if (st == 0)
  {
   mode0(0);//get data no calib
  }
  else if(st == 1)
  {
   mode1();//get data with calib
  }
  else if(st == 2)
  {
    lcd_clear();
    lcd_write(lcd_Command_mode, 0x0E);
    lcd_write_string(lcd_cursor(0,0), "Get the GPS... ");
    mode2(); //call function to get gps
    ////
    lcd_write(lcd_Command_mode, 0x0E);
    lcd_write_string(lcd_cursor(0,0), "Lat = ");
    display_float(lattitude,6,0);
    lcd_write_string(lcd_cursor(0,1), "Lon = ");
    display_float(longtitude,6,1);
    delay_ms(5000);
  }
  else if(st == 3)
  {
    lcd_init();
    lcd_write(lcd_Command_mode, 0x0E);
    lcd_write_string(lcd_cursor(0,0), "Sending Data");
    mode3();
  }
    else if(st == 4)
  {
    lcd_init();
    lcd_write(lcd_Command_mode, 0x0E);
    lcd_write_string(lcd_cursor(0,0), "Sending Data 2");
    mode3_2();
  }
  else if(st == 5)
  {
    Show_data();
  }    
  
}
void Show_lcd(uint8_t sta)
{

  if(sta == 0)
  {
    //UART_SendString("ayyo0");
    lcd_init();
    lcd_write(lcd_Command_mode, 0x0E);
    lcd_write_string(lcd_cursor(0,0), "> Get data <");
    lcd_write_string(lcd_cursor(0,1), "  Calib data");
}
/////////////////////////////////////////
else if(sta == 1)
{
  //UART_SendString("ayyo1");
    lcd_init();
    lcd_write(lcd_Command_mode, 0x0E);
  lcd_write_string(lcd_cursor(0,0), "  Get data");
  lcd_write_string(lcd_cursor(0,1), "> Calib data <");
}
/////////////////////////////////////////
else if(sta == 2)
{
  //UART_SendString("ayyo2");
    lcd_init();
    lcd_write(lcd_Command_mode, 0x0E);
  lcd_write_string(lcd_cursor(0,0), "  Calib data");
  lcd_write_string(lcd_cursor(0,1), "> Get GPS <"); 
}
/////////////////////////////////////////
else if(sta == 3)
{
  //UART_SendString("ayyo3");
    lcd_init();
    lcd_write(lcd_Command_mode, 0x0E);
  lcd_write_string(lcd_cursor(0,0), "  Get GPS");
  lcd_write_string(lcd_cursor(0,1), "> Send data <"); 
}
else if(sta == 4)
{
  //UART_SendString("ayyo4");
    lcd_init();
    lcd_write(lcd_Command_mode, 0x0E);
  lcd_write_string(lcd_cursor(0,0), "Send data");
  lcd_write_string(lcd_cursor(0,1), "> Send data2 <"); 
}
else if(sta == 5)
{
  //UART_SendString("ayyo4");
  lcd_init();
  lcd_write(lcd_Command_mode, 0x0E);
  lcd_write_string(lcd_cursor(0,0), "Send data 2");
  lcd_write_string(lcd_cursor(0,1), "> Show data <"); 
} 
}
void mode0(int state) //get data no calib
{
 unsigned int c = 0;
        int index = 0;
        for(index = 0; index < 1; index++)
       {
        val1 = 0;
        val2 = 0;
        val3 = 0;
        val4 = 0;
        val5 = 0;
        val6 = 0;
        val7 = 0;
//////////////////////////////////////////////////////////
if(state == 0)
{        
        lcd_init();
        lcd_write(lcd_Command_mode, 0x0E);
        lcd_write_string(lcd_cursor(0,0), "Measuring.......");
        lcd_write_string(lcd_cursor(0,1), "Measure Nitro");
}
        for( c = 0; c < 8; c++)
         { 
          temp_data = nitrogen();
          val1 += temp_data;
          
         }
//////////////////////////////////////////////////////////       
if(state == 0)
{  
        lcd_init();
        lcd_write(lcd_Command_mode, 0x0E);
        lcd_write_string(lcd_cursor(0,0), "Measuring.......");
        lcd_write_string(lcd_cursor(0,1), "Measure Phospho");
}
        for( c = 0; c < 8; c++)
         { 
          temp_data = phosphorous();
          val2 += temp_data;
          
         }
//////////////////////////////////////////////////////////   
if(state == 0)
{      
        lcd_init();
        lcd_write(lcd_Command_mode, 0x0E);
        lcd_write_string(lcd_cursor(0,0), "Measuring.......");
        lcd_write_string(lcd_cursor(0,1), "Measure Potas"); 
}        
        for( c = 0; c < 8; c++)
         { 
          temp_data = potassium();
          val3 += temp_data;
          
         }
//////////////////////////////////////////////////////////
if(state == 0)
{         
        lcd_init();
        lcd_write(lcd_Command_mode, 0x0E);
        lcd_write_string(lcd_cursor(0,0), "Measuring.......");
        lcd_write_string(lcd_cursor(0,1), "Measure humi");   
}
          for( c = 0; c < 8; c++)
         { 
          temp_data = humidity();
          val4 += temp_data/10;
          
         }
 //////////////////////////////////////////////////////////
 if(state == 0)
{         
        lcd_init();
        lcd_write(lcd_Command_mode, 0x0E);
        lcd_write_string(lcd_cursor(0,0), "Measuring.......");
        lcd_write_string(lcd_cursor(0,1), "Measure PH");   
}
          for( c = 0; c < 8; c++)
         { 
          temp_data = PH();
          val5 += temp_data/10;
          
         }        
 //////////////////////////////////////////////////////////      
 if(state == 0)
{   
        lcd_init();
        lcd_write(lcd_Command_mode, 0x0E);
        lcd_write_string(lcd_cursor(0,0), "Measuring.......");
        lcd_write_string(lcd_cursor(0,1), "Measure EC");  
} 
          for( c = 0; c < 8; c++)
         { 
          temp_data = EC();
          val6 += temp_data;
          
         }
 ////////////////////////////////////////////////////////// 
 if(state == 0)
{        
        lcd_init();
        lcd_write(lcd_Command_mode, 0x0E);
        lcd_write_string(lcd_cursor(0,0), "Measuring.......");
        lcd_write_string(lcd_cursor(0,1), "Measure Temp");   
}
          for( c = 0; c < 8; c++)
         { 
          temp_data = Temp();
          val7 += temp_data/10;
          
         }         
        val1 = val1/8.0f;
        val2 = val2/8.0f;
        val3 = val3/8.0f;
        val4 = val4/8.0f;
        val5 = val5/8.0f;
        val6 = val6/8.0f;
        val7 = val7/8.0f;

       }          
}
void mode1() // get data with calib
{
  int st_calib = 0;
  float N_calib = 0;
  float P_calib = 0;
  float K_calib = 0;
  float humi_calib = 0;
  float temp_data = 0;
  int index, c, c_engine;
  float sumX = 0; //humidity
  float sumXX = 0; //sum(humidity ^ 2)
  double sumXY_N = 0, sumXY_P = 0, sumXY_K = 0; //data multiplied with humidity
  double sumY_N = 0, sumY_P = 0, sumY_K = 0; //data
  int n = 5; //number of inter val
  int position = 0;
  int temp_position = 0;
  while (st_calib != 1)
  {
    for (index = 0; index < n; index +=0)
    {
     lcd_init();
     lcd_write(lcd_Command_mode, 0x0E);
     lcd_write_string(lcd_cursor(0,0), "Measure data");
     lcd_write_string(lcd_cursor(0,1), "Times: ");
     display_int(index, 6, 1);
     //start measure
      for (c = 0; c < 10; c++)
      {
        N_calib += nitrogen(); // Replace with actual function
        delay_ms(500);               
      }

      // Phosphorus calibration
      for (c = 0; c < 10; c++)
      {
        P_calib += phosphorous(); // Replace with actual function
        delay_ms(500);
      }

       //Potassium calibration
      for (c = 0; c < 10; c++)
      {
        K_calib += potassium(); // Replace with actual function
        delay_ms(500);
      }
      // Humidity calibration
      for (c = 0; c < 10; c++)
      {
        humi_calib += humidity()/10; // Replace with actual function
        delay_ms(500);
      }
       //Average calculations
      N_calib /= 10;
      P_calib /= 10;
      K_calib /= 10;
      humi_calib /= 10;
      lcd_init();
      lcd_write(lcd_Command_mode, 0x0E);

      //disp Nitrogen
      temp_position = lcd_write_string(lcd_cursor(0,0), "N=");
      position += temp_position;
      temp_position = display_float(N_calib, position, 0);
      position += temp_position; 
       
       //disp Photphorus
       temp_position = lcd_write_string(lcd_cursor(position,0), "P=");
       position += temp_position;
       position = display_float(P_calib, position, 0);

      // //disp Kali or potassium
      position = 0; //restart again

      temp_position = lcd_write_string(lcd_cursor(position,1), "K=");
      position += temp_position;
      temp_position = display_float(K_calib, position, 1);
      position += temp_position;

      //disp humidity
      temp_position = lcd_write_string(lcd_cursor(position,1), "%RH=");
      position += temp_position;
      temp_position = display_float(humi_calib, position, 1);
      position += temp_position;
      // UART2_SendFloat(N_calib);      // UART2_SendString(" \r\n ");

      // UART2_SendFloat(P_calib);      // UART2_SendString(" \r\n ");
      
      // UART2_SendFloat(K_calib);      // UART2_SendString(" \r\n ");

      // UART2_SendFloat(humi_calib);      // UART2_SendString(" \r\n ");

      uint8_t temp = 0; //temp data for while loop      
      while(temp == 0)
      {     
        BTst_1 = (P5IN & BIT1) ? 1 : 0;
        BTst_2 = (P5IN & BIT2) ? 1 : 0;
        BTst_3 = (P5IN & BIT3) ? 1 : 0;     
        if (BTst_1 != Lt_BTst_1)
        {
          if( BTst_1 == LOW) //calib again
          {
            temp = 1; //exit loop
          }
        }
          if (BTst_2 != Lt_BTst_2)
          { 
            if( BTst_2 == LOW) //continue
            {
              sumX += humi_calib;
              sumY_N += N_calib;
              sumY_P += P_calib;
              sumY_K += K_calib;

              sumXX += humi_calib * humi_calib; //sum(humidity ^ 2)

              sumXY_N += N_calib * humi_calib;
              sumXY_P += P_calib * humi_calib;
              sumXY_K += K_calib * humi_calib;



              index += 1;
              temp = 1; //exit loop
            }
          }
    /////////////////////////////////////////
    Lt_BTst_3 = BTst_3; //update Last value of Button to current status
    Lt_BTst_2 = BTst_2; //update Last value of Button to current status
    Lt_BTst_1 = BTst_1; //update Last value of Button to current status        
    }

  }
    st_calib = 1;

}
  slope_N = ((n * sumXY_N) - sumX * sumY_N ) / (n * sumXX - sumX * sumX);
  slope_P = ((n * sumXY_P) - sumX * sumY_P ) / (n * sumXX - sumX * sumX);
  slope_K = ((n * sumXY_K) - sumX * sumY_K ) / (n * sumXX - sumX * sumX);

  itc_N = (sumY_N * sumXX - sumX * sumXY_N) / (n * sumXX - sumX * sumX);
  itc_P = (sumY_P * sumXX - sumX * sumXY_P) / (n * sumXX - sumX * sumX);
  itc_K = (sumY_K * sumXX - sumX * sumXY_K) / (n * sumXX - sumX * sumX);

  N_c = slope_N * 100 + itc_N;
  P_c = slope_P * 100 + itc_P;
  K_c = slope_K * 100 + itc_K;
  lcd_init();
  lcd_write(lcd_Command_mode, 0x0E);
  lcd_write_string(lcd_cursor(0,0), "Calibrated data");
  delay_ms(1000);

  lcd_init();
  lcd_write(lcd_Command_mode, 0x0E);
  position = 0;
      //disp Nitrogen
      temp_position = lcd_write_string(lcd_cursor(0,0), "N=");
      position += temp_position;
      temp_position = display_float(N_c, position, 0);
      position += temp_position; 
       
       //disp Photphorus
       temp_position = lcd_write_string(lcd_cursor(position,0), "P=");
       position += temp_position;
       position = display_float(P_c, position, 0);

      // //disp Kali or potassium
      position = 0; //restart again

      temp_position = lcd_write_string(lcd_cursor(position,1), "K=");
      position += temp_position;
      temp_position = display_float(K_c, position, 1);
      position += temp_position;

      //disp humidity
      temp_position = lcd_write_string(lcd_cursor(position,1), "hum=");
      position += temp_position;
      temp_position = display_float(100, position, 1);
      position += temp_position;  
  delay_ms(2000);  
}
void mode2() //get gps
{
     UCA1IE |= UCRXIE;// enable uart interrupt
    delay_ms(20000);
    //Lattitude
    int lat_degree = atoi(lat_deg);
    float lat_minutes = atof(lat_min);
    float longtitude2, lattitude2;
    // Longitude
    int lon_degree = atoi(lon_deg);
    float lon_minutes = atof(lon_min);
    longtitude2 = (float)lon_degree + (lon_minutes / 60.0f);
    lattitude2 = (float)lat_degree + (lat_minutes / 60.0f);
    if(longtitude2 != 0 && lattitude2 != 0 )
    {
      longtitude = longtitude2;
      lattitude = lattitude2;
    }
    UCA1IE &= ~UCRXIE; //disable uart interrupt
}
void mode3() // send data sim7020
{
    delay_ms(10000);
  const char createHTTP[] = "AT+CHTTPCREATE=\"https://script.google.com/\"\r\n";
  const char connectHTTP[] = "AT+CHTTPCON=0\r\n";
  UART2_SendStringArray(createHTTP);
  delay_ms(10000);
  UART2_SendStringArray(connectHTTP);
  delay_ms(10000);
    // Send start of command
    UART2_SendStringArray("AT+CHTTPSEND=0,0,\"/macros/s/AKfycbxBVFDCiS-zvoqKi48bKRJc91NrBGYhXaeKpaJeFDPFOVdnSKGrm_E70vKPFVQvycyp/exec?field1=");
    UART2_SendFloat(val1);
    UART2_SendString("&field2=");    UART2_SendFloat(val2);
    UART2_SendString("&field3=");    UART2_SendFloat(val3);  
    UART2_SendString("&field4=");    UART2_SendFloat(val4);
    UART2_SendString("&field5=");    UART2_SendFloat(val5);
    UART2_SendString("&field6=");    UART2_SendFloat(val6);  
    UART2_SendString("&field7=");    UART2_SendFloat(val7);
    UART2_SendString("&field8=");    UART2_SendFloat(lattitude);
    UART2_SendString("&field9=");    UART2_SendFloat(longtitude); 
    UART2_SendChar('\"');    UART2_SendString("\r\n");
    delay_ms(10000);
}
void mode3_2() // send data sim7020
{
  const char createHTTP[] = "AT+HTTPINIT\r\n";

  UART2_SendStringArray(createHTTP);
  delay_ms(5000);
    // Send start of command

UART2_SendStringArray("AT+HTTPPARA=\"URL\",\"https://script.google.com/macros/s/AKfycbxBVFDCiS-zvoqKi48bKRJc91NrBGYhXaeKpaJeFDPFOVdnSKGrm_E70vKPFVQvycyp/exec?field1=");    
    UART2_SendFloat(val1);
    UART2_SendString("&field2=");    UART2_SendFloat(val2);
    UART2_SendString("&field3=");    UART2_SendFloat(val3);  
    UART2_SendString("&field4=");    UART2_SendFloat(val4);
    UART2_SendString("&field5=");    UART2_SendFloat(val5);
    UART2_SendString("&field6=");    UART2_SendFloat(val6);  
    UART2_SendString("&field7=");    UART2_SendFloat(val7);
    UART2_SendString("&field8=");    UART2_SendFloat(lattitude);
    UART2_SendString("&field9=");    UART2_SendFloat(longtitude); 
    //UART2_SendChar('\"');    
    UART2_SendString("\r\n");
    delay_ms(5000);
    const char action[] = "AT+HTTPACTION=0 \r\n";
    UART2_SendStringArray(action);
    delay_ms(5000);
    UART2_SendStringArray("AT+HTTPTERM\r\n");

}
void Show_data() //display lcd
{
int data_st = 1, seen = 1;
int lp = 1;
int position = 0, temp_position = 0;
lcd_init();
lcd_write(lcd_Command_mode, 0x0E);
while(lp == 1)
{
  BTst_1 = (P5IN & BIT1) ? 1 : 0;
  BTst_2 = (P5IN & BIT2) ? 1 : 0;
  BTst_3 = (P5IN & BIT3) ? 1 : 0; 
    if (BTst_1 != Lt_BTst_1)
      {
        if( BTst_1 == LOW) 
          {
            seen = 1;
            position = 0;
            data_st +=1;
            lcd_init();
            lcd_write(lcd_Command_mode, 0x0E);
            UART2_SendInt(data_st);
            if(data_st >= 12)
            {
              data_st = 0;
            }
          }
      } 
            
    Lt_BTst_3 = BTst_3; //update Last value of Button to current status
    Lt_BTst_2 = BTst_2; //update Last value of Button to current status
    Lt_BTst_1 = BTst_1; //update Last value of Button to current status
    if(seen == 1 && data_st == 1) //Nitrogen
      {
      position = 0;
      temp_position = lcd_write_string(lcd_cursor(0,0), "N = ");
      position += temp_position;
      temp_position = display_float(val1, position, 0);
      position += temp_position;
      seen = 0;
      
      }
      else if( seen == 1 && data_st == 2) //Photphorus
      {
      position = 0;
      temp_position = lcd_write_string(lcd_cursor(0,0), "P = ");
      position += temp_position;
      temp_position = display_float(val2, position, 0);
      position += temp_position;
      seen = 0;
      }
      else if( seen == 1 && data_st == 3) //Kali_Potasium
      {
      position = 0;
      temp_position = lcd_write_string(lcd_cursor(0,0), "K = ");
      position += temp_position;
      temp_position = display_float(val3, position, 0);
      position += temp_position;
      seen = 0;
      }
      else if( seen == 1 && data_st == 4) //humidity
      {
      position = 0;
      temp_position = lcd_write_string(lcd_cursor(0,0), "Humi = ");
      position += temp_position;
      temp_position = display_float(val4, position, 0);
      position += temp_position;
      seen = 0;
    }
      else if( seen == 1 && data_st == 5) //pH
      { 
      position = 0;
      temp_position = lcd_write_string(lcd_cursor(0,0), "pH = ");
      position += temp_position;
      temp_position = display_float(val5, position, 0);
      position += temp_position;
      seen = 0;
      }
      else if( seen == 1 && data_st == 6) //EC
      {
      position = 0;
      temp_position = lcd_write_string(lcd_cursor(0,0), "EC = ");
      position += temp_position;
      temp_position = display_float(val6, position, 0);
      position += temp_position;
      seen = 0;
      }
      else if( seen == 1 && data_st == 7) //temp
      {
      position = 0;
      temp_position = lcd_write_string(lcd_cursor(0,0), "Temp = ");
      position += temp_position;
      temp_position = display_float(val7, position, 0);
      position += temp_position;
      seen = 0;
      }
      else if( seen == 1 && data_st == 8)
      {
      position = 0;
      temp_position = lcd_write_string(lcd_cursor(0,0), "Lat = ");
      position += temp_position;
      temp_position = display_float(lattitude, position, 0);
      position = 0;
      seen = 0;

      position = 0;
      temp_position = lcd_write_string(lcd_cursor(0,1), "Lon = ");
      position += temp_position;
      temp_position = display_float(longtitude, position, 1);
      position += temp_position;
      seen = 0;
      }
      else if( seen == 1 && data_st == 9)
      {
      position = 0;
      temp_position = lcd_write_string(lcd_cursor(0,0), "N_calib = ");
      position += temp_position;
      temp_position = display_float(N_c, position, 0);
      position += temp_position;
      seen = 0;
      }
      else if( seen == 1 && data_st == 10)
      {
      position = 0;
      temp_position = lcd_write_string(lcd_cursor(0,0), "P_calib = ");
      position += temp_position;
      temp_position = display_float(P_c, position, 0);
      position += temp_position;
      seen = 0;
      }
      else if( seen == 1 && data_st == 11)
      {
      position = 0;
      temp_position = lcd_write_string(lcd_cursor(0,0), "K_calib = ");
      position += temp_position;
      temp_position = display_float(K_c, position, 0);
      position += temp_position; 
      seen = 0;
        delay_ms(2000);
        lp = 0;      
      }
}
}


// Timer configuration
void config_timer(void)
{
    TA0CTL = TASSEL_1 | MC_1 | TACLR; // SMCLK, Up mode, clear TAR
    TA0CCTL0 = CCIE;                  // Enable interrupt for CCR0
    TA0CCR0 = 10000;                  // Set the compare value (SMCLK/10k ~ 1ms if SMCLK = 10MHz)

    __bis_SR_register(GIE);          // Enable global interrupts
}
// void senddata(int var1, int var2, int var3)
// {
//   const char createHTTP[] = "AT+CHTTPCREATE=\"https://script.google.com/\"\r\n";
//   const char connectHTTP[] = "AT+CHTTPCON=0\r\n";
//   const char sendHTTP[] = "AT+CHTTPSEND=0,1,\"/api/device/input?api_key=Fruitsensor1\",\"\",\"application/x-www-form-urlencoded\",";
// //  UART2_SendStringArray(createHTTP, sizeof(createHTTP));
//   delay_ms(500);
//  // UART2_SendStringArray(connectHTTP, sizeof(connectHTTP));
//   delay_ms(500);
//  // UART2_SendStringArray(sendHTTP, sizeof(sendHTTP));
//   delay_ms(500);
// //HEX CODE FUNCTION
//     const char *prefix1 = "field9=";
//     const char *prefix2 = "&field10=";
//     const char *prefix3 = "&field11=";

//     char pota[100];  // final hex string
//     int idx = 0;
//     char hexpair[2];

//     // Helper macro to encode and append one char as hex
//     #define APPEND_HEX_CHAR(c) do { \
//         byte_to_hex((unsigned char)(c), hexpair); \
//         pota[idx++] = hexpair[0]; \
//         pota[idx++] = hexpair[1]; \
//     } while(0)

//     // Append prefix1
//     int i;
//     for (i = 0; prefix1[i]; i++) APPEND_HEX_CHAR(prefix1[i]);
//     APPEND_HEX_CHAR('0' + var1);

//     for ( i = 0; prefix2[i]; i++) APPEND_HEX_CHAR(prefix2[i]);
//     APPEND_HEX_CHAR('0' + var2);

//     for ( i = 0; prefix3[i]; i++) APPEND_HEX_CHAR(prefix3[i]);
//     APPEND_HEX_CHAR('0' + var3);

//     // Done: send over UART
//     UART_SendStringArray(pota, idx);
//     UART_SendString("\r\n");

// }
void toLPM3()
{
      UCA0CTL1 |= UCSWRST;  // UART0
      UCA1CTL1 |= UCSWRST;  // UART1

      // 2. Optional: Disable TX and RX pins (set as GPIO or inputs)
      P2SEL0 &= ~(BIT0 | BIT1);  // UCA0 RXD/TXD (P2.0 = RXD, P2.1 = TXD)
      P2SEL1 &= ~(BIT0 | BIT1);

      P3SEL0 &= ~(BIT4 | BIT5);  // Clear function select
      P3SEL1 &= ~(BIT4 | BIT5);;

      // 3. Optional: Set pins as inputs to reduce current draw
      P2DIR &= ~(BIT0 | BIT1);
      P2OUT &= ~(BIT0 | BIT1);   // Pull-down (or use pull-up as needed)
      P2REN |=  (BIT0 | BIT1);   // Enable pull resistor


      P3DIR &= ~(BIT4 | BIT5);
      P3OUT &= ~(BIT4 | BIT5);   // Pull-down (or use pull-up as needed)
      P3REN |=  (BIT4 | BIT5);   // Enable pull resistor

    // Set all GPIOs as input with pull-down resistors
    P1DIR = 0x00; P1REN = 0xFF; P1OUT = 0x00;
    P2DIR = 0x00; P2REN = 0xFF; P2OUT = 0x00;
    P3DIR = 0x00; P3REN = 0xFF; P3OUT = 0x00;
    P4DIR = 0x00; P4REN = 0xFF; P4OUT = 0x00;
    P5DIR = 0x00; P5REN = 0xFF; P5OUT = 0x00;
    P6DIR = 0x00; P6REN = 0xFF; P6OUT = 0x00;
    P7DIR = 0x00; P7REN = 0xFF; P7OUT = 0x00;
    P8DIR = 0x00; P8REN = 0xFF; P8OUT = 0x00;
    P9DIR = 0x00; P9REN = 0xFF; P9OUT = 0x00;
    PADIR = 0x00; PAREN = 0xFF; PAOUT = 0x00;
    PBDIR = 0x00; PBREN = 0xFF; PBOUT = 0x00;
    PCDIR = 0x00; PCREN = 0xFF; PCOUT = 0x00;
    PDDIR = 0x00; PDREN = 0xFF; PDOUT = 0x00;
    PEDIR = 0x00; PEREN = 0xFF; PEOUT = 0x00;
    PJDIR = 0x00; PJREN = 0xFF; PJOUT = 0x00;
    P10DIR = 0x00; P10REN = 0xFF; P10OUT = 0x00;
    PJSEL0 |= BIT4 | BIT5;  // Select LFXIN/LFXOUT function
    //TA0CTL = TASSEL_1 | MC_2 | TAIE | TACLR; // ACLK, continuous mode, enable overflow interrupt
   if(system_mode != 2)
   {
   __bis_SR_register(LPM3_bits | GIE);  // Enter LPM3
   }
}
int display_float(float data, uint8_t x, uint8_t y)
{
// Extract integer and fractional parts
    int position;
    int int_part = (int)data;

    // Get first digit after decimal, rounded
    int frac_part = (int)((data - int_part) * 10 + 0.5);

    // Handle negative numbers correctly
    if (data < 0) {
        int_part = (int)data;
        frac_part = (int)((-data - (-int_part)) * 10 + 0.5);
    }

    char number[8]; // Enough space for e.g. "-99.9\0"

    if (frac_part == 0)
    {
        // No decimal part: print as integer
        if (data < 0)
            sprintf(number, "-%d", -int_part);
        else
            sprintf(number, "%d", int_part);
    }
    else
    {
        // Has decimal part: print with 1 digit
        if (data < 0)
            sprintf(number, "-%d.%d", -int_part, frac_part);
        else
            sprintf(number, "%d.%d", int_part, frac_part);
    }

    lcd_write(lcd_Command_mode, 0x0E);  // Turn on cursor (optional)
    position = lcd_write_string(lcd_cursor(x, y), number);

    return position;
}
void display_int(int data, uint8_t x, uint8_t y)
{
    char buffer[5];
    sprintf(buffer, "%d", data);  // Convert integer to string

    lcd_write(lcd_Command_mode, 0x0E);
    lcd_write_string(lcd_cursor(x, y), buffer);
}
void initRTC(void) {
    RTCCTL0_H = RTCKEY_H;

       // Configure RTC in calendar mode
    RTCCTL0_L = RTCAIE; // Enable alarm interrupt
    RTCCTL1 = RTCSSEL_0 | RTCMODE; // Select ACLK (LFXT, 32.768 kHz), calendar mode
    RTCSEC = 0;
    RTCMIN = 0;
    RTCHOUR = 0;
    RTCDOW = 0x00;
    updateAlarm();

    RTCCTL1 &= ~RTCHOLD;
    RTCCTL0_H = 0;
}

void updateAlarm(void) {
    RTCCTL0_H = RTCKEY_H;

    unsigned int min = RTCMIN;
    unsigned int hour = RTCHOUR+alarm_plus;
    unsigned int day = RTCDAY;


        if (hour >= 24) {
            hour = 0;
            day++;
            if (day > 31) day = 1;
        }

    RTCAMIN = min | 0x80;   // Match minute
    RTCAHOUR = hour | 0x80; // Match hour
    RTCADAY = day | 0x80;   // Match day
    RTCADOW = 0 | 0x80;

}



// Timer_A overflow ISR
#pragma vector = TIMER0_A1_VECTOR
__interrupt void Timer0_A1_ISR(void)
{
    switch (__even_in_range(TA0IV, TA0IV_TAIFG))
    {
        case TA0IV_TAIFG:  // Timer overflow
            overflow_count++;
            if (overflow_count >= 150)  // 150 overflows ≈ 5 minutes
            {
                overflow_count = 0;
                TA0CTL &= ~TAIE; // ACLK, continuous mode, enable overflow interrupt
                __bic_SR_register_on_exit(LPM3_bits); // Wake up
            }
            UART2_SendString("Print at: ");
            UART2_SendInt(overflow_count);
            UART2_SendString("\r\n");
            break;
        default: break;
    }
}


#pragma vector=RTC_VECTOR
__interrupt void RTC_ISR(void) {
    switch (__even_in_range(RTCIV, RTCIV_RTCAIFG)) {
        case RTCIV_RTCAIFG:
            updateAlarm();
            __bic_SR_register_on_exit(LPM3_bits);
            break;
        default:
            break;
    }
}
