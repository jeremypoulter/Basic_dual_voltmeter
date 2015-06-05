/*
 *
 * Udemy.com
 * Building an Arduino DC Voltmeter
 * https://blog.udemy.com/arduino-voltmeter/
 *
 */
 
float vPow = 3.2230;
float r1 = 10000;
float r2 = 10000;
 
void setup() 
{
  pinMode(8, OUTPUT);    // initialize pin 8 to control the radio
  pinMode(4, OUTPUT);    // initialize pin 4 to wake radio
  pinMode(13, OUTPUT);   // initialize pin 13 to control the LED

  Serial.begin(115200);
  
  radioOn();
  Serial.println("--------------------"); delay(50);
  Serial.println("DC VOLTMETER"); delay(50);
  Serial.print("Maximum Voltage: "); delay(50);
  Serial.print((int)(vPow / (r2 / (r1 + r2)))); delay(50);
  Serial.println("V"); delay(50);
  Serial.println("--------------------"); delay(50);
  Serial.println(""); delay(50);
  Serial.println("\"Time\",\"Vcc\",\"PV\",\"Bat\""); delay(50);
  radioOff();
  
  delay(2000);
}
 
void loop() 
{
  vPow = (float)readVcc() / 1000.0;
  float pvVoltage = readVoltage(1);
  float batVoltage = readVoltage(2);

  radioOn();
  Serial.print(millis());
  Serial.print(",");
  Serial.print(vPow);
  Serial.print(",");
  Serial.print(pvVoltage);
  Serial.print(",");
  Serial.println(batVoltage);delay(50);
  
//  printVoltage("PV ", pvVoltage); delay(50);
//  printVoltage("Bat", batVoltage); delay(50);
//  printVoltage("Vcc", vPow); delay(50);
  radioOff();

  delay(10000);
}
 
void printVoltage(char *name, float voltage)
{
  Serial.print(name);
  Serial.print(" = ");
  Serial.println(voltage, 3);
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

void radioOn()
{
  digitalWrite(13, HIGH); // select the radio
  digitalWrite(4, LOW); // wake the radio
  delay(50);
  digitalWrite(8, HIGH); // select the radio
}

void radioOff()
{
  digitalWrite(8, LOW); // select the radio
  digitalWrite(4, HIGH); // select the radio
  digitalWrite(13, LOW); // select the radio
}

