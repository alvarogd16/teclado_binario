#include <LiquidCrystal.h>
#include <Keyboard.h>

// LCD stuff
const int rs = 4, en = 5, d4 = 9, d5 = 8, d6 = 7, d7 = 6;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// Buttons pins
const int b0 = 10, b1 = 16, bDel = 14;

byte keyCode   = 0;
int actualPos  = 7;
int lastMask = 0;
int pressed = 0;
int countdown = 0;
const int delayLong = 1000;
const int delayShort = 100;
const int pollingRate = 20;


/* Print in the LCD the number num in binary */
void printBinary(byte num){
  lcd.setCursor(0, 1);
  for(int i = 7; i >= 0; i--) {
    // Create a mask with the necessary bits and do the logical operation &
    if(num & (1 << i))
      lcd.print("1");
    else
      lcd.print("0");
  }
}

void sendKey(){
  Keyboard.write(keyCode);
  
  //lcd.clear();
  //lcd.setCursor(0, 1);
  //lcd.print(keyCode);

  keyCode = 0;
  delay(500);
  printBinary(keyCode);
}

void writeValue(int value){
  keyCode += value*(1 << actualPos);
  actualPos--;

  printBinary(keyCode);

  if(actualPos < 0){
    actualPos = 7;
    sendKey();
  }
  
  lcd.setCursor(7-actualPos, 1);
}

void deleteValue() {
  if(actualPos < 7){
    actualPos++;
    
    //If there're a 1 in the previous position, upgrade the keyCode 
    if(keyCode & (1 << actualPos)){
      keyCode -= (1 << actualPos);
    }

    printBinary(keyCode);
    
    lcd.setCursor(7-actualPos, 1);
  }
}

void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);
  pinMode(b0, INPUT_PULLUP);
  pinMode(b1, INPUT_PULLUP);
  pinMode(bDel, INPUT_PULLUP);
  
  lcd.blink();
  printBinary(keyCode);
  
  lcd.setCursor(0, 0);
  lcd.print("Teclado binario");
  lcd.setCursor(0, 1);

  Keyboard.begin();
}

void loop() {
  int mask = !digitalRead(b0)
              | !digitalRead(b1)   << 1
              | !digitalRead(bDel) << 2;

  if (mask > lastMask) {
    pressed = mask ^ lastMask;
    countdown = 0;
  } else if (mask < lastMask) {
    pressed = 0;
    countdown = 0;
  }

  if (countdown == 0) {
    switch (pressed) {
    case 1:
      writeValue(0);
      break;
    case 1 << 1:
      writeValue(1);
      break;
    case 1 << 2:
      deleteValue();
      break;
    }

    if (mask != lastMask)
      countdown = delayLong;
    else
      countdown = delayShort;
  }

  delay(pollingRate);
  countdown -= pollingRate;

  lastMask = mask;
}
