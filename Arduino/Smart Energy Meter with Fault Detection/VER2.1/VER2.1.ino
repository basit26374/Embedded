#define SS_PIN 10  //D2
#define RST_PIN 9 //D1

#include <SPI.h>
#include <MFRC522.h>
#include<EEPROM.h>
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.
String checker = " ";




void setup()
{
  Serial.begin(9600);   // Initiate a serial communication
  SPI.begin();      // Initiate  SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522
}


void loop()
{
  
  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent())
  {
    check(checker);
    return;
  }
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial())
  {
//    Serial.println("Card Detected");
//    delay(250);
    return;
  }
  //Show UID on serial monitor
  Serial.println();
  Serial.print(" UID tag :");
  String content = "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++)
  {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
    content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  content.toUpperCase();
  Serial.println();
  checker = content.substring(1);
  
  if (content.substring(1) == "50 3B B2 2B") //change UID of the card that you want to give access
  {
    Serial.println("Welcome User");
    delay(1000);
  }
  
  else
  {
    Serial.println(" Access Denied ");
    delay(3000);
  }
}

//===================== FUNCTIONS ============================





void check(String temp)
{
    if (temp == "50 3B B2 2B") //change UID of the card that you want to give access
  {
    char f[] = "";
    Serial.println("Welcome User");
    delay(1000);
  }
  
}
