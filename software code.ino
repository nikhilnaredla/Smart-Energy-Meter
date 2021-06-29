
// include the library code:
#include <LiquidCrystal.h>

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);


const long interval = 990;   
unsigned long previousMillis = 0; 
float power=0;
float energy=0;
char receivedChar;
boolean newData = false;
float joule_price = 2.93;

unsigned long t_start=0,on=0;
boolean is_on=false, is_off=true;


void setup() {
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  pinMode(13, OUTPUT);
  pinMode(A0,INPUT); // set pin a0 as input pin
  pinMode(A1,INPUT); // set pin a1 as input pin
  Serial.begin(9600); // open the serial port at 9600 bps:
}

void loop() {
  
  
   float  sensor_voltage  = get_voltage ( 200 ) ;    // 20ms
   float sensor_current = get_currentvoltage(200);   // 20ms
   
   double rms_voltage= (sensor_voltage+0.6)*(325/5)*(1/sqrt(2));
 
   String value=String(rms_voltage);
   String main="Voltage: ";
   main.concat(value); main.concat('V');
  
  
//  Serial.print(" RMS Voltage: " );
//  Serial.println(rms_voltage);
  
  double rms_current=(sensor_current-2.5)/(0.200)*(1/sqrt(2));
  
  String current="Current: ";
  current.concat(rms_current);
  
//  Serial.print("RMS Current: ");
//  Serial.println(rms_current);


  recvOneChar();
  showNewData();
  
  unsigned long currentMillis = millis();

  
 
  // this calls get_power function evry 1 sec
  if (currentMillis - previousMillis >= interval) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;
//    Serial.println(currentMillis);
    get_power(rms_voltage,rms_current);
  }

  
}

void recvOneChar() 
{
    if (Serial.available() > 0) 
    {
        receivedChar = Serial.read();
        newData = true;
    }
}

void showNewData() {
    
    if (newData == true && receivedChar=='e') 
    {   
        Serial.println();
        Serial.print("The Energy consumed is ");
        Serial.print(energy); Serial.println('J');
        float price = joule_price*(energy/1000);   //for kwHr pricing
        Serial.print("The price is Rs ");
        Serial.println(price);
        newData = false;
    }

    if (newData == true && receivedChar=='o') // turn on the load
    {
         t_start=millis();  // record start time
         is_on=true;
         is_off=false;
         Serial.println();
         Serial.println("The Load is turned ON");
         digitalWrite(13,HIGH);
         newData = false;
    }
    
    if (newData == true && receivedChar=='f') 
    { 
        Serial.println();
        Serial.println("The Load is turned OFF");
        is_on=false;
        is_off=true;
        on=on + (millis()-t_start);  //record total on time
        digitalWrite(13,LOW);
        newData = false;
    }
}

float get_power(float voltage,float current)
{
    power = voltage * current;

    if(power<3){ power=0;}
    
    if(is_on==true)  // means the load is turned on using 'o' command
    {
//      Serial.print("On Time is ");
//      Serial.println( on + (millis()-t_start) );
      energy = power * ( (on + (millis()-t_start) )/1000);
    }
    else{
//      Serial.print("On time is");
//      Serial.println(on);
//      Serial.println(power);
      energy += power*on;
    }
    
    
    String pr= "Energy:";
    pr.concat(energy);
    pr.concat('J');
    
    lcd.setCursor(0,0);
    String ti="Time:"; ti.concat(millis()/1000); ti.concat('s');
    lcd.print(ti);
    
    lcd.setCursor(0, 1);
    lcd.print(pr);
    
//    Serial.print("Power is");
//    Serial.println(power);  
}

float  get_voltage ( int  n_samples ) 
{ 
  float  max_voltage = 0, voltage=0 ;
  
  for ( int  i = 0 ; i < n_samples ; i ++ ) 
  { 
    voltage  = analogRead ( A0 )  *  ( 5.0  /  1023.0 ) ;
    
    if(voltage>max_voltage){ max_voltage=voltage;}     
  } 
  return ( max_voltage ) ; 
}

float  get_currentvoltage ( int  n_samples ) 
{ 
  float  max_voltage = 0, voltage=0 ;
  
  for ( int  i = 0 ; i < n_samples ; i ++ ) 
  { 
    voltage  = analogRead ( A1 )  *  ( 5.0  /  1023.0 ) ;
    
    if(voltage>max_voltage){ max_voltage=voltage;}     
  } 
  return ( max_voltage ) ; 
}
