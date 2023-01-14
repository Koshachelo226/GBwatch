#include "GyverTimer.h"   // подключаем библиотеку
GTimer myTimer(MS);

#include <OLED_I2C.h>
OLED  myOLED(SDA, SCL, 8);
extern uint8_t SmallFont[];
extern uint8_t RusFont[];                           

#define CLK 6
#define DT 5
#define SW 4

#include "GyverEncoder.h"
Encoder enc1(CLK, DT);

//#include <GyverEncoder.h>

int enemyScore = 0;
int playerScore = 0;

//Players
int playerY = 32;
int playerX = 3;
int enemyY = 32;
int enemyX = 124;
int ai = 5;

//Ball
int ballY = 32;
int ballX = 64;
int Bspeed = 1;
//dirX, 0-Right, 1 - Left
//dirY, 0-Down, 1-Up
bool dirX = 0;
bool dirY = 0;

void setup() {
  pinMode(2, INPUT_PULLUP);
  pinMode(4, INPUT_PULLUP);
  myTimer.setTimeout(20);
  enc1.setType(TYPE1);
  enc1.setTickMode(AUTO);
  randomSeed(analogRead(0));
  Serial.begin(9600);
  
  myOLED.begin();
  myOLED.setFont(SmallFont);
  myOLED.clrScr();
  myOLED.update();
}

void loop() {
  if (enc1.isRight() && playerY+6 < 60) {playerY += 6;}
  if (enc1.isLeft() && playerY-6 > 23) {playerY -= 6;}

  if (!digitalRead(2) == 1 && playerY+6 < 60) {playerY += 3;}
  if (!digitalRead(4) == 1 && playerY-6 > 23) {playerY -= 3;}
  
  myOLED.clrScr();
  game();
  
  myOLED.drawLine(playerX,(playerY-6), playerX,(playerY+6));
  myOLED.drawLine(enemyX,(enemyY-6), enemyX,(enemyY+6));
  drawBall(ballX,ballY);

  //Ball direction executator:
  if (dirX == 0) {ballX++; delay(Bspeed);}
  if (dirX == 1) {ballX--; delay(Bspeed);}
  if (dirY == 0) {ballY++; delay(Bspeed);}
  if (dirY == 1) {ballY--; delay(Bspeed);}

  //Ball Y selector:
  if (ballY >= 63) {dirY = 1;}
  if (ballY == 18) {dirY = 0;}

  collis();
  enemy();

  if (ballX <= 0) {ballX = 64; ballY = 32; enemyScore += 1; dirX = int(random(0,2));}
  if (ballX >= 127) {ballX = 64; ballY = 32; playerScore += 1; dirX = int(random(0,2));}
  
  myOLED.update();

}

//void direct() {}

void timer(int sec) {
    for (int i = 0; i<sec; i++) {
        if (i = (sec-1)) {return true;}
      }
  }

void game() {
    myOLED.drawLine(0,15,127,15);
    myOLED.drawLine(66,0,66,14);
    myOLED.print(String(playerScore), 32, 3);
    myOLED.print(String(enemyScore), 96, 3);
  }

void collis() {
    if (ballX == enemyX-1) {
      if (ballY > enemyY-6 && ballY < enemyY+6) {dirX = 1; dirY = int(random(0,2));} 
    } 

  if (ballX == playerX+1) {
      if (ballY > playerY-7 && ballY < playerY+7) {dirX = 0; dirY = int(random(0,2));} 
    } 
  }

void drawBall(int x,int y) {
    myOLED.setPixel(x-1, y-1);
    myOLED.setPixel(x, y-1);
    myOLED.setPixel(x+1, y-1);
    myOLED.setPixel(x-1, y);
    myOLED.setPixel(x+1, y);
    myOLED.setPixel(x-1, y+1);
    myOLED.setPixel(x, y+1);
    myOLED.setPixel(x+1, y+1);
  }

void enemy() {
  
  if (myTimer.isReady()) {
    Serial.println(ballY);
    
      if (enemyY > ballY) {
        enemyY -= 4;
        myTimer.start();
      }

      if (enemyY < ballY) {
        enemyY += 4;
        myTimer.start();
      }

      myTimer.start();
  }
  
  }
