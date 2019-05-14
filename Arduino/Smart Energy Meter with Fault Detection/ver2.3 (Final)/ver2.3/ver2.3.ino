#define SS_PIN 10  
#define RST_PIN 9 

#define RELAY1 4 
#define RELAY2 5 
#define RELAY3 6 
#define RELAY4 7 

#include <SPI.h>
#include <MFRC522.h>
#include<EEPROM.h>
#include <Math.h>
#include "EmonLib.h" 
#include<SoftwareSerial.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);
SoftwareSerial Serial2(2, 3);
EnergyMonitor emon1;
EnergyMonitor emon2;
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.
String checker = " ";

String server = ""; //variable for sending data to webpage
boolean No_IP = false; //variable to check for ip Address
String IP = ""; //variable to store ip Address
char temp1 = '0';
int a = 0;
char data;

int flag_1 = 1; // When load is ON flag=1, load is OFF when flag=0
int flag_2 = 1;
int flag_3 = 1;
int flag_4 = 1;

int user_flag_1 = 1; // When load is ON flag=1, load is OFF when flag=0
//int user_flag_2 = 1;
//int user_flag_3 = 1;
//int user_flag_4 = 1;

int main_flag = 1;

int previous_id = 0;
unsigned long thresh = 0UL;
unsigned long temper = 0UL;


//-------------------------Variable for Voltage as=nd Current Sensor data---------------
float sample1=0; // for voltage
float sample2=0; // for current_1 Household meter
float sample3=0; // for current_2 PMT meter
float voltage=0.0;
float val; // current callibration
float actualval; // read the actual current from ACS 712
float amps=0.0;
float amps_2=0.0;
float totamps=0.0; 
float avgamps=0.0;
float amphr=0.0;
float watt=0.0;
float energy=0.0;

void setup()
{
  Serial.begin(9600);   // Initiate a serial communication
  lcd.begin(20,4); // Initialize LCD
  SPI.begin();      // Initiate  SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522
  emon1.current(1, 1.2); //Initiallize Current sensor_1 for household meter
  emon2.current(0, 1.0); //Initiallize Current sensor_2 for Main PMT meter
float f1 = 50.7f;   // initial units 
//float f2 = 12.5f;
EEPROM.put(11, f1);


  while (a == 0) {
    Serial2.begin(115200);
    Serial.println("Changing Baud to 9600...");
    establishConnection("AT+CIOBAUD=9600",1500);
    //establishConnection("AT+UART_DEF=9600,8,1,0,0",1500);
    a++;
  }

  Serial2.end();
  Serial2.begin(9600);
  wifi_init();
  
  showIP();

  // -----------------Relay Board -----------------

 pinMode(RELAY1, OUTPUT); //Defining the pin 7 of the Arduino as output
 pinMode(RELAY2, OUTPUT); //Defining the pin 6 of the Arduino as output
 pinMode(RELAY3, OUTPUT); //Defining the pin 5 of the Arduino as output
 pinMode(RELAY4, OUTPUT); //Defining the pin 4 of the Arduino as output

 digitalWrite(RELAY1,HIGH);
 digitalWrite(RELAY2,HIGH);
 digitalWrite(RELAY3,HIGH);
 digitalWrite(RELAY4,HIGH);

 
  Serial.println("\n \n System Ready..");
}


void loop()
{
   
  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent())
  {
    delay(100);
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
  
 
  if (content.substring(1) == "50 3B B2 2B" and user_flag_1 == 1)
  {
     Serial.println("FIRST");
     delay(1000);
     user_flag_1 = 0;
  }
  else if (content.substring(1) == "50 3B B2 2B" and user_flag_1 == 0)
  {
     int id = 11;
     float units;
     user_flag_1 = 1;
     units = Cal_electric_para(id);
     Serial.print("UNITS CONSUMED :");
     Serial.print(energy);
     Serial.println("Kilo-Watt-Hour");
     Serial.println("");
     delay(500);
     update_units(id,units);
    Serial.println("User first Out");
    checker = "";
    Serial.println("Wait for next user");
    delay(1000);
    
  }

//    else if (content.substring(1) == "EA AB 26 1F" and user_flag_2 == 1 )
//  {
//     Serial.println("SECOND");
//     delay(1000);
//     user_flag_2 = 0;
//  }
//  else if (content.substring(1) == "EA AB 26 1F" and user_flag_2 == 0)
//  {
//     int id = 22;
//     float units;
//     user_flag_2 = 1;
//     units = Cal_electric_para(id);
//     Serial.print("UNITS CONSUMED :");
//     Serial.print(energy);
//     Serial.println("Kilo-Watt-Hour");
//     Serial.println("");
//     delay(500);
//     update_units(id,units);
//    Serial.println("User Second Out");
//    checker = "";
//    Serial.println("Wait for next user");
//    delay(1000);
//  }
//
//    else if (content.substring(1) == "EA 73 E2 1F" and user_flag_3 == 1)
//  {
//     Serial.println("THIRD");
//     delay(1000);
//     user_flag_3 = 0;
//  }
//  else if (content.substring(1) == "EA 73 E2 1F" and user_flag_3 == 0)
//  {
//     int id = 13;
//     float units;
//     user_flag_3 = 1;
//     units = Cal_electric_para(id);
//     Serial.print("UNITS CONSUMED :");
//     Serial.print(energy);
//     Serial.println("Kilo-Watt-Hour");
//     Serial.println("");
//     delay(500);
//     update_units(id,units);
//    Serial.println("User Third Out");
//    checker = "";
//    Serial.println("Wait for next user");
//    delay(1000);
//  }
//
//
//    else if (content.substring(1) == "EA E9 78 1F" and user_flag_4 == 1)
//  {
//     Serial.println("FORTH");
//     delay(1000);
//     user_flag_4 = 0;
//  }
//  else if (content.substring(1) == "EA E9 78 1F" and user_flag_4 == 0)
//  {
//     int id = 14;
//     float units;
//     user_flag_4 = 1;
//     units = Cal_electric_para(id);
//     Serial.print("UNITS CONSUMED :");
//     Serial.print(energy);
//     Serial.println("Kilo-Watt-Hour");
//     Serial.println("");
//     delay(500);
//     update_units(id,units);
//    Serial.println("User Forth Out");
//    checker = "";
//    Serial.println("Wait for next user");
//    delay(1000);
//  }

 
  else
  {
    Serial.println(" Access Denied ");
    delay(3000);
  }



}
//===================== FUNCTIONS ============================

void loadSwitch(int id)
{
  int identity = id;
  //char payload;
  //char t;
  int temp;
  delay(100);
if (main_flag == 1)
{
  
  if (Serial2.available()) 
  {
    while(Serial2.available())
    
    {
      
      //char t= Serial2.read(); 
      temp = Serial2.parseInt();
      //payload=payload+t;
      //Serial.println(t);
     }
     //Serial.println(payload);
     Serial.println((uint8_t) temp);
     Serial2.end();    // Ends the serial communication once all data is received
     Serial2.begin(9600);  // Re-establishes serial communication , this causes deletion of anything previously stored in the buffer                             //or cache
  
//============= Second Approach ================
//  if (Serial2.available())
//  {
//    temp = Serial2.parseInt();
//  }
//  Serial.println((uint8_t) temp);
//==============================================  
//  Serial2.end();    // Ends the serial communication once all data is received
//  Serial2.begin(9600);


//  if (Serial2.available() > 0)
//  {
//    payload = Serial2.read();
//    Serial.println(payload);
//  }
//    if(payload == 1)
//    {
//      Serial.println("Load 1 ON");
//    }
//    else if (payload == 2)
//    {
//      Serial.println("Load 2 ON");
//    }

    if(temp == 1 and flag_1 == 1)
    {
//      lcd.clear();
//      lcd.setCursor(0,0);
//      lcd.print("Load 1 ON");
      delay(50);
      Serial.println("Load 1 ON");
      digitalWrite(RELAY1,LOW);
      flag_1 = 0 ;
    }
    else if (temp == 1 and flag_1 == 0)
    {
//      lcd.clear();
//      lcd.setCursor(0,0);
//      lcd.print("Load 1 OFF");
      delay(50);
      Serial.println("Load 1 OFF");
      digitalWrite(RELAY1,HIGH);
      flag_1 = 1;
    }

    else if (temp == 2 and flag_2 == 1)
    {
//      lcd.clear();
//      lcd.setCursor(0,0);
//      lcd.print("Load 2 ON");
      delay(50);
      Serial.println("Load 2 ON");
      digitalWrite(RELAY2,LOW);
      flag_2 = 0;
    }
    else if (temp == 2 and flag_2 == 0)
    {
//      lcd.clear();
//      lcd.setCursor(0,0);
//      lcd.print("Load 2 OFF");
      delay(50);
      Serial.println("Load 2 OFF");
      digitalWrite(RELAY2,HIGH);
      flag_2 = 1;
    }
    
    else if (temp == 3 and flag_3 == 1)
    {
//      lcd.clear();
//      lcd.setCursor(0,0);
//      lcd.print("Load 3 ON");
      delay(50);
      Serial.println("Load 3 ON");
      digitalWrite(RELAY3,LOW);
      flag_3 = 0;
    }
    else if (temp == 3 and flag_3 == 0)
    {
//      lcd.clear();
//      lcd.setCursor(0,0);
//      lcd.print("Load 3 OFF");
      delay(50);
      Serial.println("Load 3 OFF");
      digitalWrite(RELAY3,HIGH);
      flag_3 = 1;
    }

    else if (temp == 4 and flag_4 == 1)
    {
//      lcd.clear();
//      lcd.setCursor(0,0);
//      lcd.print("Load 4 ON");
      delay(50);
      Serial.println("Load 4 ON");
      digitalWrite(RELAY4,LOW);
      flag_4 = 0;
    }
    else if (temp == 4 and flag_4 == 0)
    {
//      lcd.clear();
//      lcd.setCursor(0,0);
//      lcd.print("Load 4 OFF");
      delay(50);
      Serial.println("Load 4 OFF");
      digitalWrite(RELAY4,HIGH);
      flag_4 = 1;
    }

    else if (temp == 90)
    {
//      lcd.clear();
//      lcd.setCursor(0,0);
//      lcd.print("Recahrge mode");
      delay(50);
      Serial.println("In Recharge Function");
      recharge(identity);
    }
  }
 }
}

void update_units(int id,float units)
{
  int oldCompareValue = 11;
  float f;
  float comapareValueFloat;
  int compareValue;
  
  //Serial.println(id);
  float val = EEPROM.get(id,f);
  Serial.println(val);
  val = (float)val - (float)units;
  Serial.println(val);
  EEPROM.put(id, val);
  //EEPROM.update(id, units);
  float data = EEPROM.get(id,f);
  Serial.print("Your Reamining Units are :  ");
  Serial.println(data);
  float unit_thresh = 49.0;
//  float diff_unit = fabs(unit_thresh - data);
  Serial.println(data - unit_thresh);
  if ((data - unit_thresh) < 0.1)
  {
//    lcd.clear();
//    lcd.setCursor(0,0);
//    lcd.print("Unit are less than 1");
//    delay(50);
//    lcd.setCursor(0,1);
//    lcd.print("  Please Recharge  ");
    
    Serial.println("Please recharge");
    delay(200);
    sendData("Please recharge");
    recharge(id);
  }    
  delay(500);
}
void recharge(int check)
{
  int a = 1;
  int iter = 0;
  int units;
  float f;
  Serial.println("In Desire Function");
  float data = EEPROM.get(check,f);
//    lcd.clear();
//    lcd.setCursor(0,0);
//    lcd.print("Current units are ");
//    lcd.setCursor(17,0);
//    lcd.print(data);
    delay(50);
//  float add = data + 10.0;
  
//  data = EEPROM.read(check);
  Serial.println(data);
  delay(5000);
  while(a)
  {
    while (Serial2.available())
    {
      units = Serial2.parseInt();
      a = 0;
    }
    
  }
  float foo = (float)units + data;
  delay(20);
//    lcd.setCursor(0,1);
//    lcd.print("You Charge : ");
//    lcd.setCursor(0,13);
//    lcd.print(units);
//    lcd.setCursor(0,15);
//    lcd.print("Units");
//    lcd.setCursor(0,2);
//    lcd.print("Your New balance is:");
//    lcd.setCursor(5,3);
//    lcd.print(foo);
    delay(3000);
  EEPROM.put(check, foo);
  Serial.println(foo);
}



void check(String temp)
{
  
  if (temp == "50 3B B2 2B")
  {
    int id = 11;
    float units;
//     char f[] = "";
     //delay(100);
     loadSwitch(id);
     //Serial.println("FIRST");
     units = Cal_electric_para(id);
     Serial.print("UNITS CONSUMED :");
     Serial.print(energy);
     Serial.println("Kilo-Watt-Hour");
     Serial.println("");
     delay(500);
     update_units(id,units);
     
  }

//    else if (temp == "EA AB 26 1F")
//  {
//    int id = 22;
//    float units;
//    loadSwitch(id);
//    Serial.println("SECOND");
//     units = Cal_electric_para(id);
//     Serial.print("UNITS CONSUMED :");
//     Serial.print(energy);
//     Serial.println("Kilo-Watt-Hour");
//     Serial.println("");
//     delay(500);
//     update_units(id,units);
//   }
//
//    else if (temp == "EA 73 E2 1F")
//  {
//    int id = 13;
//    loadSwitch(id);
//     Serial.println("THIRD");
//  }
//
//    else if (temp == "EA E9 78 1F")
//  {
//    int id = 14;
//    loadSwitch(id);
//     Serial.println("FORTH");
//  }
//
//    else if (temp == "EA EC 64 1F")
//  {
//    int id = 15;
//    loadSwitch(id);
//     Serial.println("FIFTH");
//   }
//
//   else if (temp == "D3 2F 62 A3")
//  {
////    loadSwitch();
//     Serial.println("TAG");
//   }
   
}
//------------------------- ESP 8266 ---------------------------
void findIp(int time1) //check for the availability of IP Address
{
  int time2 = millis();
  while (time2 + time1 > millis())  {
    while (Serial2.available() > 0)    {
      if (Serial2.find("IP has been read"))      {
        No_IP = true;
      }
    }
  }
}

void showIP()//Display the IP Address
{
  IP = "";
  char ch = 0;
  while (1)
  {
    Serial2.println("AT+CIFSR");
    while (Serial2.available() > 0)
    {
      if (Serial2.find("STAIP,"))
      {
        delay(1000);
        Serial.print("IP Address:");
        while (Serial2.available() > 0)
        {
          ch = Serial2.read();
          if (ch == '+')
            break;
          IP += ch;
        }
      }
      if (ch == '+')
        break;
    }
    if (ch == '+')
      break;
    delay(1000);
  }
  //For display on lcd
//  lcd.setCursor(0,0);
//  lcd.print("IP  :");
//  lcd.setCursor(0,5);
//  lcd.print(IP);
//  lcd.setCursor(0,1);
//  lcd.print("PORT: 80");
    
  Serial.print(IP);
  Serial.print("Port:");
  Serial.println(80);
}

void establishConnection(String Serial2and, int timeOut) //Define the process for sending AT Serial2ands to module
{
  int q = 0;
  while (1)
  {
    Serial.println(Serial2and);
    Serial2.println(Serial2and);
    while (Serial2.available())
    {
      if (Serial2.find("OK"))
        q = 8;
    }
    delay(timeOut);
    if (q > 5)
      break;
    q++;
  }
  if (q == 8)
    Serial.println("OK");
  else
    Serial.println("Error");
}

void wifi_init() //send AT Serial2ands to module
{

  establishConnection("AT", 100);

  establishConnection("AT+CWMODE=3", 100);
  establishConnection("AT+CWQAP", 100);
  findIp(5000);
  if (!No_IP)
  {
    Serial.println("Connecting Wifi....");
    establishConnection("AT+CWJAP=\"SES_KHI\",\"12345678\"", 7000);        //provide your WiFi username and password here
  }
  else
  {
  }
  Serial.println("Wifi Connected");
  // showIP();
  establishConnection("AT+CIPMUX=1", 5000);
  establishConnection("AT+CIPSERVER=1,80", 7000);
}

void sendData(String server1)//send data to module
{
  int p = 0;
  while (1)
  {
    unsigned int l = server1.length();
    //Serial.print("AT+CIPSEND=0,");
    Serial2.print("AT+CIPSEND=0,");
    //Serial.println(l + 2);
    Serial2.println(l + 2);
    delay(100);
    Serial.println(server1);
    Serial2.println(server1);
    while (Serial2.available())
    {
      //Serial.print(Serial.read());
      if (Serial2.find("OK"))
      {
        p = 11;
        break;
      }
    }
    if (p == 11)
      break;
    delay(100);
  }
}


float Cal_electric_para(int user_id) 
{
  a = -1;
  if (user_id != previous_id)
{
  previous_id = user_id;
  long new_time = millis();
  long new_time_sec = new_time/1000;
  thresh = new_time_sec;
  Serial.println("next user come");
  Serial.println(thresh);
  //time = new_time - time;
  
}

long milisec = millis(); // calculate time in milisec
//Serial.print("Millisec :");
//Serial.println(milisec);
long time=milisec/1000; // convert time to sec
//Serial.print("Time :");
//Serial.println(time);
temper = (time - thresh);
Serial.print("Required time :");
Serial.println(temper);
if (temper < 0)
{
  temper = temper * a;
}

if (temper == 0)
{
  temper = 1;
}
time = temper;
 ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 
                      /// taking 150 samples from sensors with a inerval of 2sec and then average the samples data collected
  for(int i=0;i<2;i++)
  {
    sample1+=analogRead(A3);  //read the voltage from the sensor
    //sample2+=analogRead(A1); //read the current from sensor
    sample2+= emon1.calcIrms(1480);
    sample3+= emon2.calcIrms(1480);
    delay(2);
  }
   sample1=sample1/2; 
   sample2=sample2/2;
   sample3=sample3/2;
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 
  /////voltage calculation//////////////////////
   //voltage=4.669*2*sample1/1000; //  callibration // 3.25v from voltage div is eqv 696 in A2 reading  // multiply 2 to get actual voltage
   
   voltage = sample1 * (230.0 / 1024.0);
  /// current calculation //////////////////////
  
//   val =(5.0*sample2)/1024.0; 
//   actualval =val-2.5; // offset voltage is 2.5v 
//   amps =actualval*10; // 100mv/A from data sheet 

    amps = sample2;
    amps_2 = sample3;
    float diff_current = fabs(amps_2 - amps);
    Serial.print("Differnce : ");
    Serial.println(diff_current,6);
    if (fabs(amps_2 - amps) > 0.009)  // 0.007 and 0.006 both are working in most cases 
    {
      Serial.println("PMT Says : Theft Detected");
      sendData("Theft Detected");
    }

    float current_thresh = 0.12;
    //Serial.println(fabs(current_thresh - amps),4);
    if (fabs(current_thresh - amps) < 0.01)
    {
      delay(200);
      Serial.println("Cutoff mode");
      sendData("Cutoff mode");
      delay(300);
      main_flag = 0;
      digitalWrite(RELAY1,HIGH);
      delay(50);
      digitalWrite(RELAY2,HIGH);
      delay(50);
      digitalWrite(RELAY3,HIGH);
      delay(50);
      digitalWrite(RELAY4,HIGH);
      delay(50);
      flag_1 = 1;
      flag_2 = 1;
      flag_3 = 1;
      flag_4 = 1;
    }
    else
    {
      main_flag = 1;
    }
    
   totamps=totamps+amps; // total amps 
   avgamps=totamps/temper; // average amps
   amphr=(avgamps*temper)/3600;  // amphour
   watt =voltage*amps;    // power=voltage*current
   //energy=(watt*time)/3600;   //energy in watt hour
   energy=(watt*(float)temper)/(1000.0*60.0);   // energy in kWh
   //energy = abs(energy);
////////////////////////////////////DISPLAY IN SERIAL MONITOR//////////////////////////////////////////////////////////

lcd.clear();
lcd.setCursor(0,0);
lcd.print("VOLTAGE : ");
lcd.setCursor(11,0);
lcd.print(voltage);
Serial.print("VOLTAGE : ");
Serial.print(voltage);
Serial.println("Volt");
delay(20);

//lcd.clear();
lcd.setCursor(0,1);
lcd.print("CURRENT :");
lcd.setCursor(11,1);
lcd.print(amps);
Serial.print("CURRENT :");
Serial.print(amps);
Serial.println("Amps");
delay(20);
Serial.print("CURRENT_2 :");
Serial.print(amps_2);
Serial.println("Amps");
Serial.println();

//lcd.clear();
lcd.setCursor(0,2);
lcd.print("POWER :");
lcd.setCursor(11,2);
lcd.print(watt);
Serial.print("POWER :");
Serial.print(watt);
Serial.println("Watt");
delay(20);

//lcd.clear();
lcd.setCursor(0,3);
lcd.print("ENERGY CONSUMED :");
lcd.setCursor(11,18);
lcd.print(energy);
Serial.print("ENERGY CONSUMED :");
Serial.print(energy);
Serial.println("Watt-Hour");
Serial.println(""); // print the next sets of parameter after a blank line
delay(500); 

return energy;
}
