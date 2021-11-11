#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>

#define FIREBASE_HOST "smarthome-cb965-default-rtdb.europe-west1.firebasedatabase.app"
#define FIREBASE_AUTH "HmF93O1J2ljERnBKcxMEljOfNZnKJWvaw45tL6iA"
#define WIFI_SSID "Pavel"
#define WIFI_PASSWORD "1234567890"
bool data_received = false;
String data;
FirebaseData firebaseData;
String data1;
String receivedData;

void setup() {

  Serial.begin(115200);   
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
  }
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  
}

void loop() 
{   
    while(Serial.available())
    {
      data = Serial.readString();
      data_received = true;
    }
    if (data_received)
    {
      Firebase.pushString(firebaseData,"System Side/Stored Values", data);
      Firebase.setString(firebaseData,"System Side/Temperature,Humidty,FireDetected,GasDetected,PresenceDetected,Security,AC,Heating,Irrigation", data);
      data_received = false;
    }
    
    if(Firebase.getString(firebaseData, "App Side/Temperature,AC,Heating,Irrigation,Security,Lock")) 
    {  
      receivedData = firebaseData.stringData();
      if(!receivedData.equals("2,2,2,2,2,2"))
      {
        Firebase.setString(firebaseData,"App Side/Temperature,AC,Heating,Irrigation,Security,Lock", "2,2,2,2,2,2");
        Serial.print(receivedData);
      }
    }
    
}
