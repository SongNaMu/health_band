//sesor header
#define USE_ARDUINO_INTERRUPTS true    // Set-up low-level interrupts for most acurate BPM math.

#include <PulseSensorPlayground.h>     // Includes the PulseSensorPlayground Library.
#include "U8glib.h"                       //oled header

const int PulseWire = 0;       // PulseSensor PURPLE WIRE connected to ANALOG PIN 0
const int LED13 = 13;          // The on-board Arduino LED, close to PIN 13.
int Threshold = 550;           // Determine which Signal to "count as a beat" and which to ignore.
                               // Use the "Gettting Started Project" to fine-tune Threshold Value beyond default setting.
                               // Otherwise leave the default "550" value. 
PulseSensorPlayground pulseSensor;  // Creates an instance of the PulseSensorPlayground object called "pulseSensor"

const float referenceVolts = 3.3;
const int batteryPin = 0;

//컨트롤러, 해상도, 통신방식
U8GLIB_SH1106_128X64 u8g(U8G_I2C_OPT_NONE);  // I2C / TWI 

void draw(char* str1, char* str2) {
  u8g.setFont(u8g_font_unifont);
  u8g.drawStr( 20, 30, str1);
  u8g.drawFrame(5,5,118,59);
  
  //배터리 용량표시
  u8g.drawStr( 20, 30, str2);
  u8g.drawStr( 20, 30, "battery");
}

void setup(void) {
  Serial.begin(9600);
  if ( u8g.getMode() == U8G_MODE_R3G3B2 ) {
    u8g.setColorIndex(255);     // white
  }
  else if ( u8g.getMode() == U8G_MODE_GRAY2BIT ) {
    u8g.setColorIndex(3);         // max intensity
  }
  else if ( u8g.getMode() == U8G_MODE_BW ) {
    u8g.setColorIndex(1);         // pixel on
  }
  else if ( u8g.getMode() == U8G_MODE_HICOLOR ) {
    u8g.setHiColorByRGB(255,255,255);
  }

  pulseSensor.analogInput(PulseWire);   
  pulseSensor.blinkOnPulse(LED13);       //auto-magically blink Arduino's LED with heartbeat.
  pulseSensor.setThreshold(Threshold);
     
  if (pulseSensor.begin()) {
    Serial.println("We created a pulseSensor Object !");  //This prints one time at Arduino power-up,  or on Arduino reset.  
  }
}

void loop(void) {

  int myBPM = pulseSensor.getBeatsPerMinute();  // Calls function on our pulseSensor object that returns BPM as an "int".
                                                 // "myBPM" hold this BPM value now. 
  int val = analogRead(batteryPin);
  int volts = (referenceVolts / 1023.0) * val;
  
  
  if (pulseSensor.sawStartOfBeat()) {            // Constantly test to see if "a beat happened". 
   Serial.println("♥  A HeartBeat Happened ! "); // If test is "true", print a message "a heartbeat happened".
   Serial.print("BPM: ");                        // Print phrase "BPM: " 
   Serial.println(myBPM);                        // Print the value inside of myBPM. 
   
//int to char array
   String str1 = String(myBPM);
   int str_len1 = str1.length() + 1;
   char char_array1[str_len1];
   str1.toCharArray(char_array1, str_len1);

   String str2 = String(myBPM);
   int str_len2 = str2.length() + 1;
   char char_array2[str_len2];
   str2.toCharArray(char_array2, str_len2);
  
    u8g.firstPage();  
    do {
      draw(char_array1, char_array2);
    } while( u8g.nextPage() );
    
    // rebuild the picture after some delay
    delay(50);
   
  }

  delay(10);                    // considered best practice in a simple sketch.


//------------------------------------------------------------------------------------------------------

  // picture loop
  
  
}
