#include <SimpleDHT.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <MFRC522.h>
#include <avr/interrupt.h> 
#include <Keypad.h>
#include <SoftwareSerial.h>

int pinDHT11 = 24;
SimpleDHT11 dht11(pinDHT11);
byte temperature;
byte humidity;
LiquidCrystal_I2C lcd(0x27,20,4);
unsigned long close_lock;
bool lock_open=false;
int relay_lock = 22;
int relay_irrigation= 23;
int relay_AC=34;
int relay_heater=35;
int RST_MFRC = 49;
int SDA_MFRC = 53;
MFRC522 mfrc522(SDA_MFRC,RST_MFRC);
const byte ROWS = 4;
const byte COLS = 3; 
char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};
byte rowPins[ROWS] = {32, 31, 30, 29};
byte colPins[COLS] = {28, 27, 26};
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );
String card1 =" 93 c2 35 16";
String card2 =" c3 13 db 16";
bool right_card=false;
bool wrong_card=false;
int buzzer= 25;
unsigned long buzz1=0;
bool ring=false;
int flame_sensor = 2;
int gas_sensor = 18;
int PIR_sensor = 3;
int lcd_page=0;
bool new_page=true;
bool fire_detected=false;
bool presence_detected=false;
bool gas_detected=false;
bool enable_security=false;
bool alarm_on=false;
bool alarm_on_copy=false;
bool alarm_low=true;
bool alarm_set=false;
unsigned long alm1;
unsigned long alm2;
bool connection_status=false;
bool disarm_alarm=false;
bool fire_detected_copy=true;
bool presence_detected_copy=true;
bool gas_detected_copy=true;
bool AC_on=false;
bool heater_on=false;
bool irrigation_on=false;
char key_pressed;
int selected_temp=0;
int selected_temp_copy=0;
bool two_digit=false;
int irrigation_duration; // 30 min max
bool irrigation_on_copy=false;
unsigned long irrigation_timer;
String data;
unsigned long timer_send;
bool send_it=true;
String receivedData;

void setup() {
  Serial.begin(9600);
  Serial3.begin(115200);
  SPI.begin();
  mfrc522.PCD_Init();
  lcd.init();     
  lcd.backlight();
  pinMode(relay_lock,OUTPUT);
  digitalWrite(relay_lock,HIGH);
  pinMode(relay_irrigation,OUTPUT);
  digitalWrite(relay_irrigation,HIGH);
  pinMode(relay_AC,OUTPUT);
  digitalWrite(relay_AC,HIGH);
  pinMode(relay_heater,OUTPUT);
  digitalWrite(relay_heater,HIGH);
  pinMode(buzzer,OUTPUT);
  digitalWrite(buzzer,HIGH);
  pinMode(flame_sensor,INPUT);
  pinMode(gas_sensor,INPUT);
  pinMode(PIR_sensor,INPUT);
  attachInterrupt(1,presence_detection,RISING);
  attachInterrupt(0,fire_detection,FALLING);
  attachInterrupt(5,gas_detection,FALLING);
}

void print_lcd(){

  switch (lcd_page) {

  case 0:
    if(new_page)
    {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Temperature: ");
    lcd.setCursor(0,1);
    lcd.print("Humidity: ");
    lcd.setCursor(14,0);
    lcd.print((char)223);
    lcd.setCursor(15,0);
    lcd.print("C");
    lcd.setCursor(12,1);
    lcd.print("% RH");
    new_page=false;
    }
    lcd.setCursor(12,0);
    lcd.print((int)temperature);
    lcd.setCursor(10,1);
    lcd.print((int)humidity);
    break;

  case 1:
    if(new_page)
    {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Set Temperature: ");
    new_page=false;
    lcd.setCursor(8,1);
    lcd.print((char)223);
    lcd.setCursor(9,1);
    lcd.print("C");
    }
    lcd.setCursor(6,1);
    if(selected_temp_copy!=0)
    {
    lcd.print(selected_temp_copy);
    }
    break;

  case 2:
    if(new_page)
    {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Door: ");
    lcd.setCursor(0,1);
    lcd.print("Security: ");
    new_page=false;
    }
    if(lock_open)
    {
      lcd.setCursor(6,0);
      lcd.print("Unlocked");
    }
    else
    {
      lcd.setCursor(12,0);
      lcd.print(" ");
      lcd.setCursor(13,0);
      lcd.print(" ");
      lcd.setCursor(6,0);
      lcd.print("Locked");
    }
   
    if(enable_security)
    {
      lcd.setCursor(12,1);
      lcd.print(" ");
      lcd.setCursor(10,1);
      lcd.print("On");
    }
    else
    {
      lcd.setCursor(10,1);
      lcd.print("Off");
    }
   
    break;

  case 3:
    if(new_page)
    {
    lcd.clear();
    if(enable_security)
    {
      lcd.setCursor(4,0);
      lcd.print("DISABLE");
      lcd.setCursor(4,1);
      lcd.print("SECURITY");
    }
    else
    {
      lcd.setCursor(12,0);
      lcd.print(" ");
      lcd.setCursor(5,0);
      lcd.print("ENABLE");
      lcd.setCursor(4,1);
      lcd.print("SECURITY");
    }
    new_page=false;
    }
    break;

  case 4:
    if(new_page)
    {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("AC: ");
    lcd.setCursor(0,1);
    lcd.print("Heating: ");
    new_page=false;
    }

    if(AC_on)
    {
      lcd.setCursor(6,0);
      lcd.print(" ");
      lcd.setCursor(4,0);
      lcd.print("On");
    }
    else
    {
      lcd.setCursor(4,0);
      lcd.print("Off");
    }

    if(heater_on)
    {
      lcd.setCursor(11,1);
      lcd.print(" ");
      lcd.setCursor(9,1);
      lcd.print("On");
    }
    else
    {
      lcd.setCursor(9,1);
      lcd.print("Off");
    }
    
    break;

  case 5:
    if(new_page)
    {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Irrigation: ");
    lcd.setCursor(0,1);
    lcd.print("Timer: ");
    new_page=false;
    }

    if(irrigation_on)
    {
      int remaining=(irrigation_timer-millis())/1000;
      lcd.setCursor(14,0);
      lcd.print(" ");
      lcd.setCursor(12,0);
      lcd.print("On");
      if(remaining<1000)
      {
        lcd.setCursor(10,1);
        lcd.print(" ");
      }
      if(remaining<100)
      {
        lcd.setCursor(9,1);
        lcd.print(" ");
      }
      if(remaining<10)
      {
      lcd.setCursor(8,1);
      lcd.print(" ");
      }
      lcd.setCursor(7,1);
      if(remaining>=0)
      {
        lcd.print(remaining);
      }
    }
    else
    {
      lcd.setCursor(12,0);
      lcd.print("Off");
    }
    
    break;

  case 6:
    if(new_page)
    {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Status: ");
    lcd.setCursor(0,1);
    lcd.print("Runtime: ");
    new_page=false;
    }
    if(connection_status)
    {
      lcd.setCursor(14,0);
      lcd.print(" ");
      lcd.setCursor(8,0);
      lcd.print("Online");
    }
    else
    {
      lcd.setCursor(8,0);
      lcd.print("Offline");
    }

    lcd.setCursor(9,1);
    lcd.print(millis()/1000);
    
    break;

  case 7:
    if(new_page)
    {
    lcd.clear();
    lcd.setCursor(2,0);
    lcd.print("HAZARD ALARM");
    lcd.setCursor(1,1);
    lcd.print("Fire Detected!");
    new_page=false;
    }
    break;

  case 8:
    if(new_page)
    {
    lcd.clear();
    lcd.setCursor(1,0);
    lcd.print("SECURITY ALERT");
    lcd.setCursor(0,1);
    lcd.print("Motion Detected!");
    new_page=false;
    }
    break;

  case 9:
    if(new_page)
    {
    lcd.clear();
    lcd.setCursor(2,0);
    lcd.print("HAZARD ALARM");
    lcd.setCursor(1,1);
    lcd.print("Gas Detected !");
    new_page=false;
    }
    break;

  default:
    break;
} 
  
}

void lock()
{
  if( right_card && !lock_open )
  {
    lock_open = true;
    close_lock = millis() + 2000;
    digitalWrite(relay_lock,LOW);
  }
  if(lock_open)
  {
    if(millis() > close_lock)
    {
      digitalWrite(relay_lock,HIGH);
      lock_open=false;
      right_card=false;
      wrong_card=false;
    }
  }
}

void rfid(){
  if(mfrc522.PICC_IsNewCardPresent())
  {
    if (mfrc522.PICC_ReadCardSerial())
    { 
      String uid = "";
      for (byte i = 0; i < mfrc522.uid.size; i++) 
      {
        uid.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
        uid.concat(String(mfrc522.uid.uidByte[i], HEX));
      }
      if(uid == card2)
      {
        right_card=true;
      }
      else
      {
        wrong_card=true;
      }
    }
  }
}

void irrigation_func()
{
  if(irrigation_on)
  {
    if(!irrigation_on_copy)
    {
      irrigation_timer=millis()+irrigation_duration*60000;
    }
    if(irrigation_timer>millis())
    {
      irrigation_on_copy=irrigation_on;
      digitalWrite(relay_irrigation,LOW);
    }
    else
    {
      irrigation_on=false;
      irrigation_on_copy=false;
      digitalWrite(relay_irrigation,HIGH);
    }
  }
}

void temperature_func()
{
  if(selected_temp!=0)
  {
    //lower temperature
    if((int)temperature>selected_temp)
    {
      heater_on=false;
      digitalWrite(relay_heater,HIGH);
      AC_on=true;
      digitalWrite(relay_AC,LOW);
    }
    //rise temperature
    else if((int)temperature<selected_temp)
    {
      AC_on=false;
      digitalWrite(relay_AC,HIGH);
      heater_on=true;
      digitalWrite(relay_heater,LOW);
    }
    else if((int)temperature==selected_temp)
    {
      AC_on=false;
      digitalWrite(relay_AC,HIGH);
      heater_on=false;
      digitalWrite(relay_heater,HIGH);
    }
  }
}

void buzz()
{
  //general alarm
  if(alarm_on)
  {
    if(alarm_set)
    {
      if(alarm_low)
      {
        if(millis()>alm1)
        {
          alarm_low=false;
        }
        else
        {
          tone(buzzer,784);
        }
      }
      else
      {
        if(millis()>alm2)
        {
          alarm_low=true;
          alarm_set=false;
        }
        else
        {
          tone(buzzer,1568);
        }
      }
      alarm_on_copy=alarm_on;
    }
    else
    {
      alm1=millis()+300;
      alm2=millis()+600;
      alarm_set=true;
    }
  }
  else
  {
    if(alarm_on_copy)
    {
      noTone(buzzer);
      digitalWrite(buzzer,HIGH);
      alarm_on_copy=false;
    }
    //wrong rfid card beep
    if(wrong_card && !lock_open)
    {
      tone(buzzer,78); //1661
      wrong_card=false;
      buzz1= millis() + 500;
      ring=true;
    }
    if(ring && millis()>buzz1)
    {
      ring =false;
      noTone(buzzer);
      digitalWrite(buzzer,HIGH);
    }
  }
}

void presence_detection()
{
  if(enable_security && !presence_detected)
  {
    presence_detected=true;
  }
}

void fire_detection()
{
  if(!fire_detected)
  {
    fire_detected=true;
  }
}

void gas_detection()
{
  if(millis()>300000 && !gas_detected)
  {
    gas_detected=true;
  }
}

void detection_manager()
{
  //disarm all alarms
  if(disarm_alarm)
  {
    new_page=true;
    lcd_page=0;
    alarm_on=false;
    fire_detected=false;
    gas_detected=false;
    presence_detected=false;
    disarm_alarm=false;
    fire_detected_copy=true;
    gas_detected_copy=true;
    presence_detected_copy=true;
  }
  else
  {
      if(fire_detected && fire_detected_copy)
      {
        fire_detected_copy=false;
        new_page=true;
        lcd_page=7;
        alarm_on=true;
      }
      else if(gas_detected && gas_detected_copy)
      {
        gas_detected_copy=false;
        new_page=true;
        lcd_page=9;
        alarm_on=true;
      }
      else if(presence_detected && presence_detected_copy)
      {
        presence_detected_copy=false;
        new_page=true;
        lcd_page=8;
        alarm_on=true;
      }
  }
}

void keyboard_func()
{
  key_pressed=keypad.getKey();

  if((key_pressed=='*' || key_pressed=='#') && (fire_detected || gas_detected || presence_detected))
  {
    disarm_alarm=true;
    selected_temp_copy=0;
  }

  else if(key_pressed=='*')
  {
    selected_temp_copy=0;
    two_digit=false;
    if(lcd_page<6)
    {
      lcd_page++;
      new_page=true;
    }
    else
    {
      lcd_page=0;
      new_page=true;
    }
  }

  else if((key_pressed=='1' || key_pressed=='2') &&  lcd_page==1 && selected_temp_copy==0)
  {
    selected_temp_copy=(key_pressed-48)*10;
  }
  else if((key_pressed=='1' || key_pressed=='2' || key_pressed=='3' || key_pressed=='4' || key_pressed=='5' || key_pressed=='6' || key_pressed=='7' || key_pressed=='8' || key_pressed=='9' || key_pressed=='0') &&  lcd_page==1 && selected_temp_copy>=10 && !two_digit)
  {
    selected_temp_copy= selected_temp_copy + (key_pressed-48);
    two_digit=true;
  }
  else if(lcd_page==1 && key_pressed=='#' && selected_temp_copy>=10)
  {
    selected_temp=selected_temp_copy;
    selected_temp_copy=0;
    two_digit=false;
    lcd_page=0;
    new_page=true;
  }

  else if(lcd_page==3 && key_pressed=='#')
  {
    enable_security=not(enable_security);
    lcd_page=0;
    new_page=true;
  }
}

void pushData()
{
   if(send_it)
   {
    data="";
    data.concat(temperature);
    data.concat(", ");
    data.concat(humidity);
    data.concat(", ");
    data.concat(fire_detected);
    data.concat(", ");
    data.concat(gas_detected);
    data.concat(", ");
    data.concat(presence_detected);
    data.concat(", ");
    data.concat(enable_security);
    data.concat(", ");
    data.concat(AC_on);
    data.concat(", ");
    data.concat(heater_on);
    data.concat(", ");
    data.concat(irrigation_on);
    timer_send=millis()+2000;
    send_it=false;
   }
   else if(millis()>timer_send)
   {
    Serial3.print(data);
    send_it=true;
   }
}

void receiveData()
{
  receivedData="";
  while(Serial3.available())
  {
    receivedData = Serial3.readString();
  }
  if(!receivedData.equals(""))
  {
    connection_status=true;
    int indx1 = receivedData.indexOf(','); 
    String tempRec = receivedData.substring(0, indx1); 
    int indx2 = receivedData.indexOf(',', indx1+1 ); 
    String acRec = receivedData.substring(indx1+1, indx2); 
    int indx3 = receivedData.indexOf(',', indx2+1 ); 
    String heatingRec = receivedData.substring(indx2+1, indx3); 
    int indx4 = receivedData.indexOf(',', indx3+1 ); 
    String irrigationRec = receivedData.substring(indx3+1, indx4); 
    int indx5 = receivedData.indexOf(',', indx4+1 ); 
    String securityRec = receivedData.substring(indx4+1, indx5); 
    String lockRec = receivedData.substring(indx5+1); 

    selected_temp=tempRec.toInt();
    
    if(acRec.toInt()==1)
    {
      selected_temp=0;
      AC_on=true;
      digitalWrite(relay_AC,LOW);
      heater_on=false;
      digitalWrite(relay_heater,HIGH);
    }
    else
    {
      if(AC_on && selected_temp<10)
      {
        selected_temp=0;
        AC_on=false;
        digitalWrite(relay_AC,HIGH);
      }
    }
    
    if(heatingRec.toInt()==1)
    {
      selected_temp=0;
      AC_on=false;
      digitalWrite(relay_AC,HIGH);
      heater_on=true;
      digitalWrite(relay_heater,LOW);
    }
    else
    {
      if(heater_on && selected_temp<10)
      {
        selected_temp=0;
        heater_on=false;
        digitalWrite(relay_heater,HIGH);
      }
    }
    
    if(irrigationRec.toInt()>=5)
    {
      irrigation_on=1;
      irrigation_on_copy=false;
      irrigation_duration=irrigationRec.toInt(); 
    }
    else 
    {
      if(irrigation_on)
      {
        irrigation_on=0;
        irrigation_on_copy=false;
        digitalWrite(relay_irrigation,HIGH);
      }
    }
    
    enable_security=securityRec.toInt();
    
    if(lockRec.toInt()==1)
    {
      lock_open=true;
      close_lock = millis() + 2000;
      digitalWrite(relay_lock,LOW);
    }
  }
}

void loop() {
  dht11.read(&temperature, &humidity, NULL);
  print_lcd();
  lock();
  rfid();
  irrigation_func();
  temperature_func();
  buzz();
  detection_manager();
  keyboard_func();
  pushData();
  receiveData();
}
