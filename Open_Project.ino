#include <SPI.h>  
#include <MFRC522.h>  
#include <Servo.h>
#include <Wire.h>   
#include <LiquidCrystal_I2C.h>
  
#define SS_PIN 10  
#define RST_PIN 9  

LiquidCrystal_I2C lcd(0x27,16,2);    
Servo myservo;
boolean aCard = false;
int beep = 7;
byte people = 2;//the number of people to come in
MFRC522 rfid(SS_PIN, RST_PIN); //实例化类  
  
// 初始化数组用于存储读取到的NUID   
byte nuidPICC [2][4] = {
  {0xB0,0xC,0x2E,0x83},  
  {0x3A,0x98,0x49,0xDD}
};
int red = 5;
int green = 6;
void setup() {   
  pinMode(red,OUTPUT);
  pinMode(green,OUTPUT);
  
  lcd.init();
  lcd.backlight();
  pinMode(beep,OUTPUT);
  myservo.attach(8);
  Serial.begin(9600);  
  SPI.begin(); // 初始化SPI总线  
  rfid.PCD_Init(); // 初始化 MFRC522   

  lcd.setCursor(0,0);
  lcd.print("Welcome to");
  lcd.setCursor(0,1);
  lcd.print("Power Bank !");
  delay(1500);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Please put your");
  lcd.setCursor(0,1);
  lcd.print("card here!");
}  
   
void loop() {
  digitalWrite(red,HIGH);
  
  readCard();
  if(aCard == true){
    check();
  }
}  

void readCard(){
  // 找卡  
  if ( ! rfid.PICC_IsNewCardPresent()){
    return;
  }

  // 验证NUID是否可读  
  if ( ! rfid.PICC_ReadCardSerial()){
    return; 
  }

  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);  
  
  // 检查是否MIFARE卡类型  
  if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&    
    piccType != MFRC522::PICC_TYPE_MIFARE_1K &&  
    piccType != MFRC522::PICC_TYPE_MIFARE_4K) {  
    Serial.println("not support");  
    return;  
  }  
    
  // 将NUID保存到nuidPICC数组  
  /*for (byte i = 0; i < 4; i++) {  
    nuidPICC[i] = rfid.uid.uidByte[i];  
  }*/     
  Serial.print("Hex UID: ");  
  printHex(rfid.uid.uidByte, rfid.uid.size);  
  Serial.println();  
    
  // 使放置在读卡区的IC卡进入休眠状态，不再重复读卡  
  rfid.PICC_HaltA();  
  
  // 停止读卡模块编码  
  rfid.PCD_StopCrypto1();
  aCard = true;
}

void check(){
  boolean finded = false;
  int sec_1 = 400;
  int sec = 200;
  int sec_3 = 50;
  int counter = 0;
  
  for(byte j = 0;j < people;j++){
    for (byte i = 0; i < 4; i++) {  
      if(nuidPICC[j][i] == rfid.uid.uidByte[i]){
        counter++;
      }
    }
    if(counter == 4){
      finded = true;
    }else{
      finded = false;
      counter = 0;
    }
  }
  
  if(counter == 4){
    Serial.println("OKOKOKOKOKOK!!!");
    digitalWrite(red,LOW);
    digitalWrite(green,HIGH);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Correct Card!");
    
    for(int i = 0;i <=100;i++){
      digitalWrite(beep,HIGH);
      delayMicroseconds(sec_1);
      digitalWrite(beep,LOW);
      delayMicroseconds(sec_1);
    }
    delay(200);
    for(int i = 0;i <=100;i++){
      digitalWrite(beep,HIGH);
      delayMicroseconds(sec);
      digitalWrite(beep,LOW);
      delayMicroseconds(sec);
    }
    delay(200);
    for(int i = 0;i <=100;i++){
      digitalWrite(beep,HIGH);
      delayMicroseconds(sec_3);
      digitalWrite(beep,LOW);
      delayMicroseconds(sec_3);
    }
    
    
    doorOpen();
    delay(4000);//+++++++----------------------------+++++++keep open time
    digitalWrite(red,HIGH);
    digitalWrite(green,LOW);
    doorClose();
    
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Welcome to");
    lcd.setCursor(0,1);
    lcd.print("Power Bank !");
    delay(1500);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Please put your");
    lcd.setCursor(0,1);
    lcd.print("card here!");
    
  }else{
    Serial.println("NONONONONONO!!!");
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Incorrect Card!");
    lcd.setCursor(0,1);
    lcd.print("Try another one!");
    for(int i = 0;i <=100;i++){
      digitalWrite(beep,HIGH);
      delayMicroseconds(sec);
      digitalWrite(beep,LOW);
      delayMicroseconds(sec);
    }
    delay(100);
    for(int i = 0;i <=100;i++){
      digitalWrite(beep,HIGH);
      delayMicroseconds(sec);
      digitalWrite(beep,LOW);
      delayMicroseconds(sec);
    }
    delay(100);
    for(int i = 0;i <=100;i++){
      digitalWrite(beep,HIGH);
      delayMicroseconds(sec);
      digitalWrite(beep,LOW);
      delayMicroseconds(sec);
    }
    
    delay(1500);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Please put your");
    lcd.setCursor(0,1);
    lcd.print("card here!");
  }
  aCard = false;
}
  
void printHex(byte *buffer, byte bufferSize) {  
  for (byte i = 0; i < bufferSize; i++) {  
    Serial.print(buffer[i], HEX);   
  }  
}  
   
void doorOpen(){
  for (int pos = 90; pos <= 155; pos += 1) { // goes from 0 degrees to 180 degrees
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
  }
}

void doorClose(){
  for (int pos = 155; pos >= 90; pos -= 1) { // goes from 180 degrees to 0 degrees
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
  }
}

