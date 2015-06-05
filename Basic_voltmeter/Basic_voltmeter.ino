/*
 *
 * Udemy.com
 * Building an Arduino DC Voltmeter
 * https://blog.udemy.com/arduino-voltmeter/
 *
 */
 
float vPow = 3.3;
float r1 = 10000;
float r2 = 10000;
 
void setup() 
{
  Serial.begin(115200);
  
  Serial.println("--------------------");
  Serial.println("DC VOLTMETER");
  Serial.print("Maximum Voltage: ");
  Serial.print((int)(vPow / (r2 / (r1 + r2))));
  Serial.println("V");
  Serial.println("+-------+-------+-------+--------+---------+");
  Serial.println("| Bat   | PV    | Vcc   | PV-Bat | Vcc-Bat |");
  Serial.println("+-------+-------+-------+--------+---------+");
  
  delay(2000);
}
 
void loop() 
{
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

  delay(1000);
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

