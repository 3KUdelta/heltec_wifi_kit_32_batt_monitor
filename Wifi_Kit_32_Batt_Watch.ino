/* Hardware Specs: Heltec_WIFI_Kit_32, 80MHz, 115200, SLAB_USBtoUART*/

#include <U8g2lib.h> // see https://github.com/olikraus/u8g2/wiki/u8g2reference

U8G2_SSD1306_128X64_NONAME_F_HW_I2C   u8g2(U8G2_R0, 16, 15, 4);

int analogPin = A4;              // Wifi Kit 32 shows analog value of voltage in A4
int val = 0;                     // variable to store the value read

//******************************** needed for running average (smoothening the analog value)
const int numReadings = 32;     // the higher the value, the smoother the average
int readings[numReadings];      // the readings from the analog input
int readIndex = 0;              // the index of the current reading
int total = 0;                  // the running total
int average = 0;                // the average

char buf[128];

void setup()
{
  Serial.begin(9600);
  
  // Initialize the graphics library.
  u8g2.begin();
  u8g2.setFont(u8g2_font_6x10_tf);
  u8g2.setFontRefHeightExtendedText();
  u8g2.setDrawColor(1);
  u8g2.setFontPosTop();
  u8g2.setFontDirection(0);
  
  for (int thisReading = 0; thisReading < numReadings; thisReading++) //used for smoothening
  {
    readings[thisReading] = 0;
  }
}

void loop()
{
  u8g2.clearBuffer();                           // Clear the display buffer.
 
  total = total - readings[readIndex];          // subtract the last reading
  readings[readIndex] = analogRead(analogPin);  // read from the sensor
  total = total + readings[readIndex];          // add the reading to the total
  readIndex = readIndex + 1;                    // advance to the next position in the array
 
  if (readIndex >= numReadings)                 // if we're at the end of the array...
  {  
    readIndex = 0;                              // ...wrap around to the beginning
  }
  val = total / numReadings;                    // calculate the average
  
  sprintf (buf, "Analog read: %d", val);
  u8g2.drawStr(1, 3, buf);
  
  if (val>=320){val=320;}                       // 3.7v 500 mAh Batt - estimated max at 320, cut at 320

  val = (val-100)/2.2;                          // calculate percentage  

  sprintf (buf, "Percent: %d", val);
  u8g2.drawStr(1, 15, buf);

  drawbatt(val);
  u8g2.sendBuffer();                            // Send the display buffer to the Blynk.run();

delay(100); 
}

void drawbatt(int percent)                      // Draw a little batt icon and fill with percentage
{
   u8g2.drawFrame(0,57,12,6);
   u8g2.drawBox(13,58,1,4);
   percent = percent/10;                        // one line per 10%  
   u8g2.drawBox(1,58,percent+1,4);              // Draw percentage within batt frame
}
