#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <U8g2lib.h>

#include <OneWire.h> 
#include <DallasTemperature.h>

#include <EEPROM.h>

#define upplim 700
#define lowlim 200
#define tick 5
#define interval 10000

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0);

OneWire wire1(2);
OneWire wire2(3);

DallasTemperature sen1(&wire1);
DallasTemperature sen2(&wire2);

uint32_t st1, st2;
void setup() {
  u8g2.begin();
  u8g2.setFont(u8g2_font_helvR14_tf);
  u8g2.setContrast(15);

  sen1.begin();
  sen2.begin();

  //pinMode(2, INPUT_PULLUP);

  pinMode(7, INPUT_PULLUP);
  pinMode(8, INPUT_PULLUP);
  pinMode(9, INPUT_PULLUP);
  pinMode(10, INPUT_PULLUP);
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);
 
  digitalWrite(13, LOW);
  digitalWrite(12, LOW);
  
  EEPROM.get(0, st1);
  EEPROM.get(4, st2);
  
}

String format(float value) {
  String result = String((int)(value+100)-100) + String(".") + String(((int)(value*10))%10);
  return result;
}

void u8drawstring(int x, int y, String str) {
  char tmp[20] = {0,};
  str.toCharArray(tmp, str.length()+1);
  u8g2.drawStr(x, y, tmp);
}

void u8drawstring(int x, int y, char* str) {
  u8g2.drawStr(x, y, str);
}

int potno = 1;
float t1, t2;

int nobtn = true;

uint32_t last;

void loop() {
  if (digitalRead(7) == LOW) {
    if (nobtn == true) {
      nobtn = false;
      potno = potno == 1 ? 2 : 1;
    }
  } else if (digitalRead(10) == LOW) {
    if (nobtn == true) {
      nobtn = false;
      potno = potno == 1 ? 2 : 1;
    }
  } else if (digitalRead(8) == LOW && nobtn == true) {
    nobtn = false;
    if (potno == 1) {
      if (st1+tick <= upplim) {
        st1 += tick;
        EEPROM.put(0, st1);
      }
    } else {
      if (st2+tick <= upplim) {
        st2 += tick;
        EEPROM.put(4, st2);
      }
    }
    //delay(20);
  } else if (digitalRead(9) == LOW && nobtn == true) {
    nobtn = false;
    if (potno == 1) {
      if (st1-tick >= lowlim) {
        st1 -= tick;
        EEPROM.put(0, st1);
      }
    } else {
      if (st2-tick >= lowlim) {
        st2 -= tick;
        EEPROM.put(4, st2);
      }
    }
    //delay(20);
  } else {
    nobtn = true;
    if (millis() - last > interval) {
      last = millis();
      u8g2.firstPage();
      u8drawstring(0, 14, F("Reading"));
      u8g2.nextPage();
      sen1.requestTemperatures();
      sen2.requestTemperatures();
      t1 = sen1.getTempCByIndex(0);
      t2 = sen2.getTempCByIndex(0);
    }
  }

  
  if (t1 * 10 > st1) {
    digitalWrite(12, HIGH);
  } else {
    digitalWrite(12, LOW);
  }
  if (t2 * 10 > st2) {
    digitalWrite(13, HIGH);
  } else {
    digitalWrite(13, LOW);
  }

  u8g2.firstPage();
  u8drawstring(0, 14, F("RicePot  #"));
  u8drawstring(88, 14, String(potno));
  u8drawstring(0, 37, F("Cur T: "));
  u8g2.drawUTF8(105, 37, "°C");
  u8drawstring(0, 60, F("Set T: "));
  u8g2.drawUTF8(105, 60, "°C");

  if (potno == 1) {
    u8drawstring(70, 37, format(t1));
    u8drawstring(70, 60, format(st1/10.0));
  } else {
    u8drawstring(70, 37, format(t2));
    u8drawstring(70, 60, format(st2/10.0));
  }
  u8g2.nextPage();

  
  //delay(500);
}
