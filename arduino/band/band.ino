//sesor header
#define USE_ARDUINO_INTERRUPTS true    // Set-up low-level interrupts for most acurate BPM math.

#include <PulseSensorPlayground.h>     // Includes the PulseSensorPlayground Library.
#include "U8glib.h"                       //oled header
#include <SoftwareSerial.h>
SoftwareSerial BTSerial(9, 8);

const int PulseWire = 0;       // PulseSensor PURPLE WIRE connected to ANALOG PIN 0
const int LED13 = 13;          // The on-board Arduino LED, close to PIN 13.
int Threshold = 550;           // Determine which Signal to "count as a beat" and which to ignore.
// Use the "Gettting Started Project" to fine-tune Threshold Value beyond default setting.
// Otherwise leave the default "550" value.
PulseSensorPlayground pulseSensor;  // Creates an instance of the PulseSensorPlayground object called "pulseSensor"

float vout = 0.0;

float vin = 0.0;

float R1 = 30000.0;

float R2 = 7500.0;

int value = 0;

//컨트롤러, 해상도, 통신방식
U8GLIB_SH1106_128X64 u8g(U8G_I2C_OPT_NONE);  // I2C / TWI

//사용자 지정 전역변수
int current_time, last_time = 0;
//최저 심박수
const int low_heartbeat = 40;
//최저심박수 유지시간

const int low_heartbeat_time = 10;
//사용자 위험 상태 위험시:1 아닐시:0
int user_state = 0;

//스위치 핀넘머
const int switchPin = 6;


int lastSwitchState = 0;
//채터링 레이턴시
int switchRatency = 50;
unsigned long switchTime;
//코드상 스위치 상태
int switchState = 0;

// 스위치 눌린 시간
unsigned long switchPushed = 0;
//x초동안 스위치 누르면 경고
int switchPushTime = 5;

//SMS 전송 상태
int smsState = 0;

void draw(char* str1, char* str2) {
    u8g.setFont(u8g_font_unifont);

    if (user_state == 0) {
        //심박수 표시
        u8g.drawStr( 20, 30, str1);

        //배터리 용량표시
        u8g.drawStr( 60, 50, str2);
        u8g.drawStr( 85, 50, "%");
        u8g.drawStr( 60, 30, "battery");
    } else {
        //경고 문고
        u8g.drawStr( 20, 30, "Warning");
    }
    //화면 테두리
    u8g.drawFrame(5, 5, 118, 59);
}

void setup(void) {
    BTSerial.begin(9600);
    Serial.begin(9600);

    pinMode(A1, INPUT);
    pinMode(switchPin, INPUT_PULLUP);

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
        u8g.setHiColorByRGB(255, 255, 255);
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
    value = analogRead(A1);

    vout = (value * 5.0) / 1023.0;  //전압값을 계산해주는 공식입니다.

    vin = vout / ( R2 / ( R1 + R2) );

    int bp = map(vin, 0.0, 4.2, 0, 100);
    //블루투스 시간 받기---------------------------------------------------------------
   
    if(BTSerial.available()){ //블루투스로 데이터가 왔다면
      char timeData = BTSerial.read();
      Serial.println(timeData);
    }
    
    //--------------------------------------------------------------------------------------------------- 
    //스위치-----------------------------------------------------------------
    int reading = digitalRead(switchPin);

    if (reading != lastSwitchState) {
        switchTime = millis();
    }
    if ((millis() - switchTime) > switchRatency) {
        if (reading != switchState) {
            switchState = reading;
            //버튼 때졌을때
            if (switchState == 1) {
                smsState = 0;
                //버튼 눌렸을때
            } else {
                switchPushed = millis();

            }
        }
    }
    lastSwitchState = reading;

    //버튼에 손을 때면 버튼 누른 시간 초기화
    if (switchState == 1) {
        switchPushed = millis();
    }
  /*  if (switchState == 0) {
        Serial.print("버튼상태 : ");
        Serial.print(switchState);
        Serial.print(", ");
        Serial.println(millis() - switchPushed);
    }
      Serial.print("버튼 상태 : ");
        Serial.print(switchState);
        Serial.print(", ");
        Serial.println(digitalRead(switchPin));
    */
    if (((millis() - switchPushed) / 1000) > switchPushTime && smsState == 0) {
        BTSerial.println(8000);
        Serial.println(8000);
        smsState = 1;
    }
    /*  if(switchState == 1){
        Serial.println("sibal");
        }*/

    //------------------------------------------------------------------------
    //Serial.print("V: ");

    //Serial.println(vin); //현재1.5V 4채널 건전지의 전압값을 출력해줍니다.
    current_time = millis() / 4000;

    if (pulseSensor.sawStartOfBeat()) {            // Constantly test to see if "a beat happened".
        //Serial.println("♥  A HeartBeat Happened ! "); // If test is "true", print a message "a heartbeat happened".
        //Serial.print("BPM: ");                        // Print phrase "BPM: "
        //Serial.println(myBPM);                        // Print the value inside of myBPM.

        //블루투스 데이터 전송 첫번째 파라미터: 심박수
        BTSerial.print(myBPM);



        //int to char array
        String str1 = String(myBPM);
        int str_len1 = str1.length() + 1;
        char char_array1[str_len1];
        str1.toCharArray(char_array1, str_len1);

        String str2 = String(bp);
        int str_len2 = str2.length() + 1;
        char char_array2[str_len2];
        str2.toCharArray(char_array2, str_len2);

        u8g.firstPage();
        do {
            draw(char_array1, char_array2);
        } while ( u8g.nextPage() );

        // rebuild the picture after some delay
        delay(50);

    }

    delay(10);                    // considered best practice in a simple sketch.
}


//------------------------------------------------------------------------------------------------------
