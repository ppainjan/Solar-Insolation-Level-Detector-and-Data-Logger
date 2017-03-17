#include <SD.h>
#include <DS1307RTC.h>
#include <Time.h>
#include <Wire.h>
#include <LiquidCrystal.h>
#include <Servo.h>

LiquidCrystal lcd(2, 3, 4, 7,9,10);
Servo hor_servo, ver_servo;
const int hor_read = A2, ver_read = A3;
int hor_pos=90, ver_pos=45, hor_ldr=0, ver_ldr=0;
float hor_vtg=0, ver_vtg=0;
int CS_pin = 8;
const int sensorValue=A1; 
int volt=0;
int s,d,m,y,h,mn;

void setup()
{
  Serial.begin(9600);
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  //lcd.print("D:");
  //lcd.setCursor(0, 1);
  lcd.print("T:");
  lcd.setCursor(0, 1);
  lcd.print("Op:");
   lcd.setCursor(7, 1);
  lcd.print("mV");
  
  hor_servo.attach(5);
  ver_servo.attach(6);
  hor_servo.write(hor_pos);
  ver_servo.write(ver_pos);
  
  

 Serial.println("Initializing Card");
  pinMode(CS_pin, OUTPUT);
  
  if (!SD.begin(CS_pin))
  {
      Serial.println("Card Failure");
      return;
  }
  
  Serial.println("Card Ready");
  File logFile = SD.open("LOG.csv", FILE_WRITE);
  if (logFile)
  {
    logFile.println(", , , , , ,"); 
    String header = "Date,Month,Year,Hour,Minute,Sensor o/p(mV)";
    logFile.println(header);
    logFile.close();
    Serial.println(header);
  }
  else
  {
    Serial.println("Couldn't open log file");
  }
}


void loop()
{

   do
  {
     hor_ldr=analogRead(hor_read);
  hor_vtg = hor_ldr*(5.0/1023.0);
  lcd.setCursor(10,0);
  lcd.print(hor_vtg);
  
  ver_ldr=analogRead(ver_read);
  ver_vtg = ver_ldr*(5.0/1023.0);
   lcd.setCursor(10,1);
  lcd.print(ver_vtg);
  
    if ((hor_vtg<2.76)&&(ver_vtg<2.34))
    {
      hor_pos=hor_pos-1;
       ver_pos=ver_pos+1;
       
    }
   
     if((hor_vtg>2.80)&&(ver_vtg>2.38))
    {
      hor_pos=hor_pos+1;
      ver_pos=ver_pos-1;
    }
    if ((hor_vtg>2.76)&&(hor_vtg<2.80)&&(ver_vtg<2.34))
    {
      ver_pos=ver_pos+1;
    }
     if ((hor_vtg>2.76)&&(hor_vtg<2.80)&&(ver_vtg>2.38))
    {
      ver_pos=ver_pos-1;
    }
     if ((ver_vtg>2.34)&&(ver_vtg<2.38)&&(hor_vtg<2.76))
    {
      hor_pos=hor_pos-1;
    }
     if ((ver_vtg>2.34)&&(ver_vtg<2.38)&&(hor_vtg>2.80))
    {
      hor_pos=hor_pos+1;
    }
    hor_servo.write(hor_pos);
    delay(250);
     ver_servo.write(ver_pos);
  delay(250);
    
  
   
  }  while(((hor_vtg<2.77|| hor_vtg >2.79)&& (ver_vtg <2.35 || ver_vtg >2.37)) /*&& (hor_pos <=175 && hor_pos >=5)&& (ver_pos <=155 && ver_pos >=20)*/);
 if((hor_pos >=175 || hor_pos <=5)&& (ver_pos >=155 || ver_pos <=20))
 {
  hor_pos = 90;
  ver_pos=45;
  hor_servo.write(hor_pos);
  delay(500);
  ver_servo.write(ver_pos);
 }
  delay(500);
  
  int sensor = analogRead(sensorValue);
   volt = sensor *(5000.0/1023.0);
 //  Serial.print(volt);
 // Serial.println(sensor);
   lcd.setCursor(3, 1);
  lcd.print(volt);  
  
    tmElements_t tm;
    
  if (RTC.read(tm))
   {
    d=tm.Day;
    m=tm.Month;
    y=tmYearToCalendar(tm.Year); 
    h=tm.Hour;
    mn=tm.Minute;
    
    lcd.setCursor(2,0);
    /*lcd.print(d);
    lcd.print('/');
    lcd.print(m);
    lcd.print('/');
    lcd.print(y);
    */
    lcd.setCursor(2,0);
    lcd.print(h);
    lcd.print(':');
    lcd.print(mn);
    }
  else
    {
      if (RTC.chipPresent())
      {
        lcd.clear();
        lcd.print("Stopped!!!");
      }
      else
      {
        lcd.clear();
        lcd.print("Error in ckt!!!");
      }
    delay(500);
}
  
  
     
String dataString = String(d) + ", " + String(m)+", " + String(y)+", " + String(h)+", " + String(mn)+", " + String(volt); 

  Serial.print(dataString); 
  File logFile = SD.open("LOG.csv", FILE_WRITE);
  if (logFile)
  {
    logFile.println(dataString);
    logFile.close();
  }
  else
  {
    lcd.clear();
    lcd.print("Couldn't open log file");
  }
  delay(180000);
}

