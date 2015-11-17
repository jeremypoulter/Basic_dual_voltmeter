/*
 *
 * Udemy.com
 * Building an Arduino DC Voltmeter
 * https://blog.udemy.com/arduino-voltmeter/
 *
 * Power saving and RFM12 code mostly stolen from emonTH
 */

#define RF69_COMPAT 1                                                              // Set to 1 if using RFM69CW or 0 is using RFM12B
#include <RFu_JeeLib.h>                                                            // https://github.com/flabbergast/RFu_jeelib.git

#include <avr/power.h>
#include <avr/sleep.h>

ISR(WDT_vect) { Sleepy::watchdogEvent(); }  // Attached JeeLib sleep function to Atmega328 watchdog -enables MCU to be put into sleep mode inbetween readings to reduce power consumption 

#define RF_freq RF12_868MHZ         // Frequency of RF12B module can be RF12_433MHZ, RF12_868MHZ or RF12_915MHZ. You should use the one matching the module you have.
const int nodeID = 2;               // RFM12B node ID - has to be unique on network
const int networkGroup = 210;       // RFM12B wireless network group - needs to be same as emonBase and emonGLCD

// PIn allocations
const int LED = 9;
const int BATT_ADC = 1;
const int PV_ADC = 2;

float vPow = 3.3;
float r1 = 10000;
float r2 = 10000;

typedef struct {                                                      // RFM12B RF payload datastructure
  int moisture;    
  int vcc;
  int battery;
  int pv;          	                                      
} Payload;
Payload packet;

void setup() 
{
  //################################################################################################################################
  // Power Save  - turn off what we don't need - http://www.nongnu.org/avr-libc/user-manual/group__avr__power.html
  //################################################################################################################################
  ACSR |= (1 << ACD);                     // disable Analog comparator    
//  power_usart0_disable();   //disable serial UART
  power_twi_disable();                    //Disable the Two Wire Interface module.
//  power_timer0_disable();
  power_timer1_disable();

  pinMode(LED, OUTPUT);
  digitalWrite(LED, HIGH);                       // Status LED on

  Serial.begin(9600);
  
  Serial.println("--------------------");
  Serial.println("DC VOLTMETER");
  Serial.print("Maximum Voltage: ");
  Serial.print((int)(vPow / (r2 / (r1 + r2))));
  Serial.println("V");

  rf12_initialize(nodeID, RF_freq, networkGroup);                       // Initialize RFM12B
  rf12_sleep(RF12_SLEEP);
  power_spi_disable();
  
  Serial.println("+-------+-------+-------+--------+---------+");
  Serial.println("| Bat   | PV    | Vcc   | PV-Bat | Vcc-Bat |");
  Serial.println("+-------+-------+-------+--------+---------+");
  delay(100);
  
  digitalWrite(LED, LOW);
}
 
void loop() 
{
  digitalWrite(LED, HIGH); //blink led while sending

  vPow = (float)readVcc() / 1000.0;
  float vBat = readVoltage(1);
  float vPV = readVoltage(2);

  Serial.print("| ");
  Serial.print(vBat, 3);
  Serial.print(" | ");
  Serial.print(vPV, 3);
  Serial.print(" | ");
  Serial.print(vPow, 3);
  Serial.print(" | ");
  Serial.print(vPV - vBat, 3);
  Serial.print("  | ");
  Serial.print(vBat - vPow, 3);
  Serial.println("   |");
  delay(100);
  
  packet.moisture = 0;
  packet.vcc = (int)(vPow * 1000);
  packet.battery = (int)(vBat * 1000);
  packet.pv = (int)(vPV * 1000);

  //make the rf transmission
  power_spi_enable();  
  rf12_sleep(RF12_WAKEUP);
  rf12_sendNow(0, &packet, sizeof packet);
  // set the sync mode to 2 if the fuses are still the Arduino default
  // mode 3 (full powerdown) can only be used with 258 CK startup fuses
  rf12_sendWait(2);
  rf12_sleep(RF12_SLEEP);
  power_spi_disable();  
  
  digitalWrite(LED, LOW);  

  dodelay(10000);
}

void dodelay(unsigned int ms)
{
  byte oldADCSRA=ADCSRA;
  byte oldADCSRB=ADCSRB;
  byte oldADMUX=ADMUX;
      
  Sleepy::loseSomeTime(ms); // JeeLabs power save function: enter low power mode for x seconds (valid range 16-65000 ms)
      
  ADCSRA=oldADCSRA;         // restore ADC state
  ADCSRB=oldADCSRB;
  ADMUX=oldADMUX;
}

float readVoltage(int pin)
{
  float v = (analogRead(pin) * vPow) / 1024.0;
  float v2 = v / (r2 / (r1 + r2));
  return v2;
}
 
 // From https://code.google.com/p/tinkerit/wiki/SecretVoltmeter
long readVcc() 
{
  long result;
  // Read 1.1V reference against AVcc
  ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  delay(2); // Wait for Vref to settle
  ADCSRA |= _BV(ADSC); // Convert
  while (bit_is_set(ADCSRA,ADSC));
  result = ADCL;
  result |= ADCH<<8;
  result = 1126400L / result; // Back-calculate AVcc in mV
  return result;
}

