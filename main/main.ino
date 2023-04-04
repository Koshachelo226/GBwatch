#include <GyverTimer.h>
//#include <DS1307RTC.h>
#include <NewPing.h>
#include <GyverEncoder.h>
#include <LiquidCrystal_I2C.h>
#include "GyverEncoder.h"
#include <OLED_I2C.h>
OLED OLED(SDA, SCL, 8);
extern uint8_t SmallFont[];
extern uint8_t RusFont[];

#define TRIGGER_PIN  12  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN     11  // Arduino pin tied to echo pin on the ultrasonic sensor.
#define MAX_DISTANCE 200
#define CLK 6
#define DT 5
#define SW 4

//bool alFlag = 0;
//bool rangeAnimStatus = 0;
//int alMin1 = 51;
//int alHour1 = 18;
//int alMin2 = 45;
//int alHour2 = 19; 
//int rangeAnim = 4;
//int hours = 0;
//int mins = 0;
//int hours1 = 0;
//int mins1 = 0;
int menuSelect = 0;
int menu = 0;
bool laserStatus = 0;
byte selectGames = 0;
//byte games = 0;

int enemyScore = 0;
int playerScore = 0;

//Players
int playerY = 32;
int playerX = 3;
int enemyY = 32;
int enemyX = 124;
//int ai = 5;

//Ball
int ballY = 32;
int ballX = 64;
//dirX, 0-Right, 1 - Left
//dirY, 0-Down, 1-Up
bool dirX = 0;
bool dirY = 0;

Encoder enc1(CLK, DT, SW);
LiquidCrystal_I2C lcd(0x27,20,4);
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // NewPing setup of pins and maximum distance.
GTimer Timer(MS);
GTimer myTimer(MS);
GTimer myTimer2(MS);

void rangeMenu() {
    exitMenu();
    displayUpdate();
    
    lcd.setCursor(0,0);
    lcd.print("Range:");

    lcd.setCursor(7,0);
    lcd.print(sonar.ping_cm());
    
}

void exitMenu() {
    if (enc1.isDouble()) {menu = 0; menuSelect = 0; lcd.clear(); OLED.clrScr();}
  }

void displayUpdate() {if (Timer.isReady()) {lcd.clear(); Timer.start();}}

void laserMenu() {
    exitMenu();
    displayUpdate();
    
    lcd.setCursor(5,0);
    lcd.print("Laser");

    if (enc1.isSingle()) {laserStatus = !laserStatus; delay(50);}

    digitalWrite(8, laserStatus);

    if (laserStatus == 0) {
      lcd.setCursor(5,1);
      lcd.print(">   |");
      }

    if (laserStatus == 1) {
      lcd.setCursor(5,1);
      lcd.print(">---|");
      }
}

void drawBall(int x,int y) {
    OLED.setPixel(x-1, y-1);
    OLED.setPixel(x, y-1);
    OLED.setPixel(x+1, y-1);
    OLED.setPixel(x-1, y);
    OLED.setPixel(x+1, y);
    OLED.setPixel(x-1, y+1);
    OLED.setPixel(x, y+1);
    OLED.setPixel(x+1, y+1);
}

void collis() {
    if (ballX == enemyX-1) {
      if (ballY > enemyY-6 && ballY < enemyY+6) {dirX = 1; dirY = int(random(0,2));} 
    } 
}

void gamePong() {
  OLED.drawLine(0,15,127,15);
  OLED.drawLine(66,0,66,14);
  OLED.print(String(playerScore), 32, 3);
  OLED.print(String(enemyScore), 96, 3);
}

void game() {
    OLED.drawLine(0,15,127,15);
    OLED.drawLine(66,0,66,14);
    OLED.print(String(playerScore), 32, 3);
    OLED.print(String(enemyScore), 96, 3);
  }

void enemy() {
  
  if (myTimer.isReady()) {
    
      if (enemyY > ballY) {
        enemyY -= 2;
        myTimer.start();
      }

      if (enemyY < ballY) {
        enemyY += 2;
        myTimer.start();
      }

      myTimer.start();
  }
  
  }

void pong() {
    exitMenu();
   if (!digitalRead(2) == 1 && playerY+6 < 60 &&  myTimer2.isReady()) {playerY += 1; myTimer2.start();}
   if (!digitalRead(3) == 1 && playerY-6 > 23 &&  myTimer2.isReady()) {playerY -= 1; myTimer2.start();}
  
  OLED.clrScr();
  game();
  
  OLED.drawLine(playerX,(playerY-6), playerX,(playerY+6));
  OLED.drawLine(enemyX,(enemyY-6), enemyX,(enemyY+6));
  drawBall(ballX,ballY);

  //Ball direction executator:
  if (dirX == 0) {ballX++;}
  if (dirX == 1) {ballX--;}
  if (dirY == 0) {ballY++;}
  if (dirY == 1) {ballY--;}
  delay(1);

  //Ball Y selector:
  if (ballY >= 60) {dirY = 1;}
  if (ballY == 20) {dirY = 0;}

  collis();
  enemy();

  if (ballX <= 0) {ballX = 64; ballY = 32; enemyScore += 1; dirX = 0;}
  if (ballX >= 127) {ballX = 64; ballY = 32; playerScore += 1; dirX = 1;}
  
  OLED.update();

}

void game1() {
    exitMenu();
    pong();
}

void setup() {
  Serial.begin(9600);

  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);

  myTimer.setTimeout(5);
  myTimer2.setTimeout(2);
  //randomSeed(analogRead(0));

  OLED.begin();
  OLED.setFont(SmallFont);
  OLED.clrScr();
  OLED.update();
  
  Timer.setTimeout(1000);
  enc1.setTickMode(AUTO);
  
  pinMode(8, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(10, OUTPUT);
  
  lcd.init();
  lcd.backlight();
  //intro();
  
} 

void loop() {
  OLED.clrScr();
  //tmElements_t tm; 
  //int realH = tm.Hour;
  //int realM = tm.Minute;

  //if (realH == alHour1 && realM == alMin1) {alFlag = 1;}
  //if (alFlag == 1) {alarmON();}
  //if (alFlag == 0) {
  if (menu == 0) {
    if (enc1.isRight() && menuSelect < 3) {menuSelect++; Serial.println("Right"); lcd.clear();}
    if (enc1.isLeft() && menuSelect > 0) {menuSelect--; Serial.println("Left"); lcd.clear();}
  }
  if (menu == 0) {selectMenu(); digitalWrite(8, 0);  digitalWrite(7, 0);}
  if (menu == 1) {laserMenu(); digitalWrite(7, 0);}
  if (menu == 2) {rangeMenu(); digitalWrite(7, 0);}
  if (menu == 3) {game1(); digitalWrite(7, 1);}
  //if (menu == 4) {alarmON(); digitalWrite(7, 0);}

  //digitalWrite(2, SW);
  //digitalWrite(3, DT);
  //digitalWrite(10, CLK);
  }

void selectMenu() {
  //tmElements_t tm;
        
  if (enc1.isDouble()) {menuSelect = 0;}
    switch (menuSelect) {
      case 0:
      //if (RTC.read(tm)) {
          displayUpdate();
          lcd.setCursor(1,0);
          lcd.print("12:00:00 AM");
          lcd.setCursor(1,1);
          lcd.print("01/01/23");
          OLED.clrScr();

          //Serial.println("HUY");
          //Serial.println(tm.Hour + tm.Minute + tm.Second);
      //}
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


/*void alarmON() {
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
  }*/

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
