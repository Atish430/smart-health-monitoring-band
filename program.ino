
#include <SoftwareSerial.h>
#include<LiquidCrystal.h>

LiquidCrystal lcd(13, 12, 11, 10, 9, 8);//8,9,10,11,12,13);//
SoftwareSerial wifiSerial(2, 3);      // RX, TX for ESP8266
int TempPin = A0;    // select the input pin for the potentiometer
int x_axis = A1;
int y_axis = A2;
int z_axis = A3;
int pul = A5;
bool DEBUG = true;   //show more logs
int responseTime = 10; //communication timeout
int cnt;
int Accel,Acce2,Acce3,pulse;
float TempC, TempF;
String sendToWifi(String command, const int timeout, boolean debug);
String sendToUno(String command, const int timeout, boolean debug);
void setup()
{


  lcd.begin(16,2);
    // Open serial communications and wait for port to open esp8266:
  Serial.begin(115200);
  lcd.setCursor(2,0);
    lcd.print("Connecting to");
    lcd.setCursor(6,1);
    lcd.print("Wifi");
     
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  wifiSerial.begin(9600);
  while (!wifiSerial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  delay(1000);
  //sendToWifi("AT+CWMODE=2",responseTime,DEBUG); // configure as access point
  //sendToWifi("AT+CIFSR",responseTime,DEBUG); // get ip address
  sendToWifi("AT+CIPMUX=1",responseTime,DEBUG); // configure for multiple connections
  sendToWifi("AT+CIPSERVER=1,3179",responseTime,DEBUG); // turn on server on port 80
//   lcd.setCursor(0,0);
//    lcd.print("Position:");
//      lcd.print(Accel);
//  lcd.setCursor(0,1);
//    lcd.print("Temp:");
//      lcd.print(Temp);
  
  sendToUno("Wifi connection is running!",responseTime,DEBUG);
  
    delay(1000);
   
  }



void loop()
{


      Accel = analogRead(x_axis);
      Acce2 = analogRead(y_axis);
      Acce3 = analogRead(z_axis);
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("x : ");
      lcd.print(Accel);
      lcd.setCursor(9,0);
      lcd.print("y : ");
      lcd.print(Acce2);
      lcd.setCursor(5,1);
      lcd.print("z : ");
      lcd.print(Acce3);
      
      delay(2000);

      lcd.clear();

      TempC = (analogRead(TempPin)/2.048);
      TempF = ((TempC) * 9.0 / 5.0) + 32.0;
      lcd.setCursor(0,0);
      lcd.print("Temp : ");
      lcd.print(TempF);
      lcd.print(" F");

      pulse = (analogRead(pul)*4.88)/25;
      lcd.setCursor(0,1);
      lcd.print("Pulse : ");
      lcd.print(pulse);
      lcd.print(" BPM");
      
      delay(2000);

    lcd.clear();
    
        
     String message="Temp : " + String(TempF) + " F"; 
     String message1="x_axis : " + String(Accel);
     String message2="y_axis : " + String(Acce2);
     String message3="z_axis : " + String(Acce3);
     String message4="Pulses : " + String(pulse) + " BPM";
      sendData(message);
      sendData(message1);
      sendData(message2);
      sendData(message3);
      sendData(message4);        
     /*String result = sendToWifi(message,responseTime,DEBUG);
      if(find(result,"OK"))
        sendData("\nOK");
      else
        sendData("\nEr");
        
  if(Serial.available()>0){
     String message = readSerialMessage();
    if(find(message,"debugEsp8266:")){
      String result = sendToWifi(message.substring(13,message.length()),responseTime,DEBUG);
      if(find(result,"OK"))
        sendData("\nOK");
      else
        sendData("\nEr");
    }
  }
  if(wifiSerial.available()>0){
    
    String message = readWifiSerialMessage();
    Serial.println(message);
    if(find(message,"Set"))
    {
    //  int ind=message.indexOf('*');
      //Serial.println(ind);
      String s1=message.substring(14,16);
      String s2=message.substring(17,19);
     String s3=message.substring(20,22);
      Serial.println(s1);
       Serial.println(s2);
        Serial.println(s3);
       message="";
     }
   }
   */
  delay(responseTime);
}


/*
* Name: sendData
* Description: Function used to send string to tcp client using cipsend
* Params: 
* Returns: void
*/
void sendData(String str){
  String len="";
  len+=str.length();
  sendToWifi("AT+CIPSEND=0,"+len,responseTime,DEBUG);
  delay(100);
  sendToWifi(str,responseTime,DEBUG);
  delay(100);
 // sendToWifi("AT+CIPCLOSE=5",responseTime,DEBUG);
}


/*
* Name: find
* Description: Function used to match two string
* Params: 
* Returns: true if match else false
*/
boolean find(String string, String value){
  return string.indexOf(value)>=0;
}


/*
* Name: readSerialMessage
* Description: Function used to read data from Arduino Serial.
* Params: 
* Returns: The response from the Arduino (if there is a reponse)
*/
String  readSerialMessage(){
  char value[100]; 
  int index_count =0;
  while(Serial.available()>0){
    value[index_count]=Serial.read();
    index_count++;
    value[index_count] = '\0'; // Null terminate the string
  }
  String str(value);
  str.trim();
  return str;
}



/*
* Name: readWifiSerialMessage
* Description: Function used to read data from ESP8266 Serial.
* Params: 
* Returns: The response from the esp8266 (if there is a reponse)
*/
String  readWifiSerialMessage(){
  char value[100]; 
  int index_count =0;
  while(wifiSerial.available()>0){
    value[index_count]=wifiSerial.read();
    index_count++;
    value[index_count] = '\0'; // Null terminate the string
  }
  String str(value);
  str.trim();
  return str;
}



/*
* Name: sendToWifi
* Description: Function used to send data to ESP8266.
* Params: command - the data/command to send; timeout - the time to wait for a response; debug - print to Serial window?(true = yes, false = no)
* Returns: The response from the esp8266 (if there is a reponse)
*/
String sendToWifi(String command, const int timeout, boolean debug){
  String response = "";
  wifiSerial.println(command); // send the read character to the esp8266
  long int time = millis();
  while( (time+timeout) > millis())
  {
    while(wifiSerial.available())
    {
    // The esp has data so display its output to the serial window 
    char c = wifiSerial.read(); // read the next character.
    response+=c;
    }  
  }
  if(debug)
  {
    Serial.println(response);
  }
  return response;
}

/*
* Name: sendToUno
* Description: Function used to send data to Arduino.
* Params: command - the data/command to send; timeout - the time to wait for a response; debug - print to Serial window?(true = yes, false = no)
* Returns: The response from the esp8266 (if there is a reponse)
*/
String sendToUno(String command, const int timeout, boolean debug){
  String response = "";
  Serial.println(command); // send the read character to the esp8266
  long int time = millis();
  while( (time+timeout) > millis())
  {
    while(Serial.available())
    {
      // The esp has data so display its output to the serial window 
      char c = Serial.read(); // read the next character.
      response+=c;
    }  
  }
  if(debug)
  {
    Serial.println(response);
  }
  return response;
}
