#include <LiquidCrystal.h>
#include <SoftwareSerial.h>
SoftwareSerial mySerial(8,9);
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
int messageCount = 0;
String message_1 = "", message_2 = "";
void setup() {
  Serial.begin(115200);
  mySerial.begin(115200);
  pinMode(6, OUTPUT);
  analogWrite(6, 0);
  lcd.begin(16, 2);
}

void loop() {
  if (mySerial.available() > 0) {
    String s = mySerial.readStringUntil('\n');
    Serial.println(s);
    messageCount++;
    if (messageCount % 2 == 0) {
      message_2 = s+"  ";
    } else message_1 = s+"  ";
    
  };
  lcd.setCursor(1,0);
  lcd.print(message_1);
  lcd.setCursor(1, 1);
  lcd.print(message_2);
  if (message_1.length() > 15 || message_2.length() > 15) lcd.scrollDisplayLeft();
  delay(1000);
}
