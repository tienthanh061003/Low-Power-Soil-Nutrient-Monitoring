# Low-Power-Soil-Nutrient-Monitoring
This project is to create a device to measure soil nnutrients: Nitrogen, Phosphorus, Potassium in long period of time with low power consumption. Store data via IoT methods
Compiler: CCS, MCU: MSP430FR5989
LCD: LCD1602
Database: Google Spreadsheet
IoT protocols: HTTP GET methods
IoT Communication device: SIM7020E
# Operating modes
This device come up with three main modes: Ultra sleep mode, Normal sleep mode (Automation mode), and Debug mode
The device start up with Automation mode, while start up, press button 2 and 3 for Ultra sleep. press 1 ad 2 for Debug mode
Automation mode: Start up and perform measuring, get GPS coordinates, send data to server and sleep for 1 hours\
Ultra sleep mode: Same as Automation mode but sleep time is 6 hours
Debug mode: Display users GUI to access all function manually
#System PCB
We also include project schematics and PCBs. Schematics and PCB include MCU PCB and SIM7020 PCB.
