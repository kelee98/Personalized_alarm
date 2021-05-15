#include <ESP8266WiFi.h>
//#include <Wire.h>      
#include <LiquidCrystal_I2C.h>        // LCD 2004 I2C용 라이브러리

LiquidCrystal_I2C lcd(0x27,20,4);     // 접근주소: 0x3F or 0x27


char ssid[] = "SK_WiFiGIGADB3C";    // your network SSID (name)
char pass[] = "1810012994";          // your network password
int status = WL_IDLE_STATUS;       // the Wifi radio's status

//Ubidots information
#define APIKEY    "1234567890"
#define ROUTEID "234000012"
#define STATIONID "206000299" 


//Hardware Pin status

char server[] = "openapi.gbis.go.kr";



boolean readingVal;
boolean readingVal1;
boolean readingVal2;
boolean readingVal3;
boolean viewData;
boolean getIsConnected = false;
String locationNo1;
String locationNo2;
String predictTime1;
String predictTime2;

String rcvbuf;

// Initialize the Ethernet client object
WiFiClient client;

void httpRequest();
void printWifiStatus();

void setup()
{
  Serial.begin(9600); 
  WiFi.begin(ssid, pass);
   lcd.init();                      // LCD 초기화
  lcd.backlight();                // 백라이트 켜기


  while ( WiFi.status() != WL_CONNECTED ) {
    delay ( 500 );
    Serial.print ( "." );
  }

  Serial.println("You're connected to the network");

  printWifiStatus();
}

void loop() {
  // if there's incoming data from the net connection send it out the serial port
  // this is for debugging purposes only
  while (client.available()) {
   
    char c = client.read();
    if ( c != NULL ) {
      if (rcvbuf.length() > 1300)
        rcvbuf = "";
      rcvbuf += c;
      Serial.write(c);
      //Serial.print(rcvbuf);
       if (rcvbuf.endsWith("<predictTime1>")) {
        readingVal = true;
        predictTime1 = "";
        Serial.println("..");
      }
      else if (rcvbuf.endsWith("<predictTime2>")) {
        readingVal1 = true;
        predictTime2 = "";
      }

      if (readingVal) 
        dataParser(c, predictTime1, readingVal);
      else if (readingVal1) 
        dataParser(c, predictTime2, readingVal1);
     
    }
  }

  //if (millis() - lastConnectionTime > postingInterval) {
 
    httpRequest();
    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print("[1st] after ");
    lcd.print(predictTime1);
    Serial.println(predictTime1);
    lcd.print("m");
    delay(5000);
    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print("[2nd] after ");
    lcd.print(predictTime2);
    Serial.println(predictTime2);
    lcd.print("m");
    delay(5000);
  
  rcvbuf = "";
}

// this method makes a HTTP connection to the server
void httpRequest() {
  Serial.println();

  // close any connection before send a new request
  // this will free the socket on the WiFi shield
  client.stop();

  // if there's a successful connection
  if (client.connect(server, 80)) {
    Serial.println("Connecting...");
    // send the HTTP PUT request
    client.print(F("GET /ws/rest/busarrivalservice?serviceKey="));
    client.print(APIKEY);
    client.print(F("&routeId="));
    client.print(ROUTEID);
    client.print(F("&stationId="));
    client.print(STATIONID);
    client.print(F(" HTTP/1.1\r\n"));
    client.print(F("Host: openapi.gbis.go.kr\r\n"));
    client.print(F("Connection: close\r\n"));
    client.print(F("\r\n\r\n"));

  }
  else {
    // if you couldn't make a connection
    Serial.println("Connection failed");
  }
}


void printWifiStatus() {
  // print the SSID of the network you're attached to
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength
  long rssi = WiFi.RSSI();
  Serial.print("Signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}

void dataParser(char c, String &data, boolean &b) {
  if (c != '<') {
    if (c != '>')
      data += c;
  }
  else {
    b = false;
  }

}
