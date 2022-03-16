// Include Arduino Wire library for I2C
#include <Wire.h> 
// Include LCD display library for I2C
#include <LiquidCrystal_I2C.h>
// Include Keypad library
#include <Keypad.h>

#include <SPI.h>
#include <RFID.h>
#define SS_PIN 10
#define RST_PIN 9
RFID rfid(SS_PIN, RST_PIN);
String rfidCard;

// Constants for row and column sizes in keypad (4x4)
const byte ROWS = 4;
const byte COLS = 4;

char data[5];
byte count = 0;
int amount;
int led = 0;

// Array to represent keys on keypad
char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

// Connections to Arduino
byte rowPins[ROWS] = {8, 7, 6, 5};
byte colPins[COLS] = {4, 3, 2, 1};

// Create keypad object
Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

// Create LCD object
LiquidCrystal_I2C lcd(0x27, 16, 2); 
//LCD Connection
// lcd sda = A4
// lcd SCL = A5
// vcc = vcc
// gnd = gnd
void setup(){
  pinMode(led , OUTPUT);
  digitalWrite(led, LOW);
   // Setup LCD with backlight and initialize
   lcd.backlight();
   lcd.init();
   //Serial.begin(9600); 
   //Serial.println("Starting the RFID Reader...");
   SPI.begin();
   rfid.init();
}

void loop(){
  lcd.clear();
  lcd.setCursor(4, 0);//lcd.setCursor(col, row);
  lcd.print("Smart Gas");
  lcd.setCursor(1, 1);
  lcd.print("Filling Station");
  delay(100);
  
  if (rfid.isCard()) {
    if (rfid.readCardSerial()) {
      rfidCard = String(rfid.serNum[0]) + " " + String(rfid.serNum[1]) + " " + String(rfid.serNum[2]) + " " + String(rfid.serNum[3]);
      //Serial.println(rfidCard);
      if (rfidCard == "23 201 122 28") {
        //Serial.println("RFID Reader: Card accepted!");
        lcd.clear();
        lcd.print("Card accepted!");
        delay(1500);
        lcd.clear();
        lcd.print("Enter amount:");
        
         while(1){
         char customKey = customKeypad.getKey();
        
         if (customKey){
           
            lcd.print(customKey);
            //Serial.println(customKey);
            data[count] = customKey;
            count++;
            
            if(customKey == 'C' && count > 0){
            lcd.setCursor(13, 0);
            lcd.print("   ");
            lcd.setCursor(13, 0);
              count = 0;
           }
            
          }
          
          if(customKey == '*'){
            count--;
            amount = num();
            //Serial.println(amount);
            lcd.clear();
            
            lcd.print("Amount =");
            lcd.print(amount);
            delay(1500);
            int time = amount * 40;//time calculation
            
            digitalWrite(led, HIGH);//for motor or lcd to represent pump is stopped
            
            lcd.clear();
            lcd.print("Filing.");
            
            int a = 0;
            while(a < time){//delay time with an animation effect
              delay(500);
              lcd.print(".");
              delay(500);
              lcd.print(".");
              delay(500);
              
              lcd.setCursor(7, 0);
              lcd.print("  "); 
              lcd.setCursor(7, 0);
              a = a + 1500;
            }
            
            //LOW
            digitalWrite(led, LOW);//for motor or lcd to represent pump is stopped
            
            lcd.clear();
            lcd.print("  Done Filling");
            lcd.setCursor(3, 1);
            lcd.print("Thank You!");
            delay(3000);
            
            break;
          }
        }
      }
      else {
       // Serial.println("RFID Reader: Card Denied!");
        lcd.clear();
        lcd.print(" Card Denied!!");
        delay(3000);
        lcd.clear();
      }
    }
    rfid.halt();
  }
}

int num()//converts data-array into number
 {
    int n = 0;
    int i = 0;
     while(i < count){
       n = n * 10 + data[i] - '0';
       i++;
     }
     count = 0;
     return n;
 }
