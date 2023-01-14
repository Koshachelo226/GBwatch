#include <GyverTimer.h>
#include <DS1307RTC.h>
#include <NewPing.h>

#define CLK 6
#define DT 5
#define SW 4
#define laserPin 8

bool alFlag = 0;
int alMin1 = 51;
int alHour1 = 18;
int alMin2 = 45;
int alHour2 = 19; 
int rangeAnim = 4;
bool rangeAnimStatus = 0;
int hours = 0;
int mins = 0;
int hours1 = 0;
int mins1 = 0;
int menuSelect = 0;
int menu = 0;
int laserStatus = 0;

#define TRIGGER_PIN  12  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN     11  // Arduino pin tied to echo pin on the ultrasonic sensor.
#define MAX_DISTANCE 200

#include "GyverEncoder.h"
Encoder enc1(CLK, DT, SW);

#include <GyverEncoder.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,20,4);
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // NewPing setup of pins and maximum distance.
GTimer Timer(MS);

byte ranger[] = {
  B10000,
  B11110,
  B10001,
  B10001,
  B10001,
  B10001,
  B11110,
  B10000
};

byte wave[] = {
  B00000,
  B10010,
  B01001,
  B01001,
  B01001,
  B01001,
  B10010,
  B00000
};

byte waveBack[] = {
  B00000,
  B01001,
  B10010,
  B10010,
  B10010,
  B10010,
  B01001,
  B00000
};

void setup() {

  lcd.createChar(0, ranger);
  lcd.createChar(1, wave);
  lcd.createChar(2, waveBack);
  Timer.setTimeout(1000);

  Serial.begin(9600);
  
  digitalWrite(laserPin,1);
  pinMode(laserPin, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(10, OUTPUT);
  
  
  enc1.setTickMode(AUTO);
  
  lcd.init();
  lcd.backlight();
  intro();
  
} 

void loop() {
  tmElements_t tm;
  
  int realH = tm.Hour;
  int realM = tm.Minute;
  Serial.println(alFlag);
  if (realH == alHour1 && realM == alMin1) {alFlag = 1;}
  if (alFlag == 1) {alarmON();}
  if (alFlag == 0) {
  if (menu == 0) {
    if (enc1.isRight() && menuSelect < 3) {menuSelect++; /*Serial.println("Right");*/ lcd.clear();}
    if (enc1.isLeft() && menuSelect > 0) {menuSelect--; /*Serial.println("Left");*/ lcd.clear();}
  }
  if (menu == 0) {selectMenu(); digitalWrite(laserPin, 0);  digitalWrite(7, 0);}
  if (menu == 1) {laserMenu(); digitalWrite(7, 0);}
  if (menu == 2) {rangeMenu(); digitalWrite(7, 0);}
  if (menu == 3) {game(); digitalWrite(7, 1);}
  if (menu == 4) {alarmON(); digitalWrite(7, 0);}

  digitalWrite(2, SW);
  digitalWrite(3, DT);
  digitalWrite(10, CLK);
  }
}



void selectMenu() {
  tmElements_t tm;
  if (enc1.isDouble()) {menuSelect = 0;}
    switch (menuSelect) {
      case 0:
      if (RTC.read(tm)) {
          displayUpdate();
          lcd.setCursor(1,0);
          lcd.print(tm.Hour);
          lcd.setCursor(7,0);
          lcd.print(tm.Minute);
          lcd.setCursor(13,0);
          lcd.print(tm.Second);
          lcd.setCursor(2,1);
          lcd.print(tm.Day);
          lcd.setCursor(5,1);
          lcd.print("/");
          lcd.setCursor(7,1);
          lcd.print(tm.Month);
          lcd.setCursor(9,1);
          lcd.print("/");
          lcd.setCursor(11,1);
          lcd.print(tm.Year);
      }
      break;
      

      case 1:
      if (enc1.isSingle()) {menu = 1;}
      lcd.setCursor(0,0);
      lcd.print("Laser");
      break;

      case 2:
      if (enc1.isSingle()) {menu = 2;}
      lcd.setCursor(0,0);
      lcd.print("Rangefinder");
      break;

      case 3:
      lcd.setCursor(0,0);
      lcd.print("OLED Game");
      if (enc1.isSingle()) {menu = 3;}
      break;

      /*case 4:
      lcd.setCursor(0,0);
      lcd.print("Alarms");
      if (enc1.isSingle()) {menu = 4;}
      break;*/
    }

}

void laserMenu() {
    exitMenu();
    displayUpdate();
    
    lcd.setCursor(5,0);
    lcd.print("Laser");

    if (enc1.isSingle()) {laserStatus = !laserStatus; delay(50);}

    digitalWrite(laserPin, laserStatus);

    if (laserStatus == 0) {
      lcd.setCursor(5,1);
      lcd.print(">   |");
      }

    if (laserStatus == 1) {
      lcd.setCursor(5,1);
      lcd.print(">---|");
      }
}

void rangeMenu() {
    exitMenu();
    displayUpdate();
    
    lcd.setCursor(0,0);
    lcd.print("Range:");

    lcd.setCursor(7,0);
    lcd.print(sonar.ping_cm());

    /*lcd.setCursor(3,1);
    lcd.write(0);
    lcd.setCursor(12,1);
    lcd.print("|");

    lcd.setCursor(5,1);
    lcd.print(")");

    lcd.setCursor(7,1);
    lcd.print("(");;

    lcd.setCursor(9,1);
    lcd.print(")");

    lcd.setCursor(11,1);
    lcd.print("(");*/
    
  }

void game() {
    exitMenu();
    lcd.setCursor(0,0);
    lcd.print("Insert Cartridge");
    lcd.setCursor(0,1);
    lcd.print("Look on screen");
  }

void alarmON() {
    if (alFlag == 1) {
        digitalWrite(3, HIGH);
        displayUpdate();
        exitMenu();
        lcd.setCursor(5,0);
        lcd.print("Alarm!");
        lcd.setCursor(5,1);
        lcd.print(alHour1);
        lcd.setCursor(7,1);
        lcd.print(alMin1);

        if (enc1.isSingle()) {alFlag = 0;}
      }
  }

void exitMenu() {
    if (enc1.isDouble()) {menu = 0; menuSelect = 0; lcd.clear();}
  }

void intro() {
    lcd.setCursor(0,0);
    lcd.print("GBT Lab Inc 1976");
    delay(1000);
    lcd.setCursor(0,1);
    lcd.print("Starting GBT_OS");
    delay(1000);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("...");
    delay(1000);
    lcd.setCursor(3,0);
    lcd.print("...");
    delay(1000);
    lcd.setCursor(0,0);
    lcd.clear();
    for (int i=0; i< 16000; i+= 100) {
       lcd.clear();
       lcd.print("RAM Test:"+ String(i));
     }
    delay(600);
    lcd.clear();
    lcd.print("CPU Freq: 16 Mhz"); 
    delay(1000);
    lcd.clear();
    lcd.print("Pls, don't crash");
    lcd.setCursor(7,1);
    lcd.print(":)");
    delay(1000);
    lcd.clear();
  }
  
void displayUpdate() {if (Timer.isReady()) {lcd.clear(); Timer.start();}}
