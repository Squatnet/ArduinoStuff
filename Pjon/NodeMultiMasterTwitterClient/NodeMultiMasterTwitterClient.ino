#include <PJONSlave.h>
#include <TwitterApi.h>
#include <Arduino.h>
/*******************************************************************
 *  An Example of getting data back from the TwitterAPI            *
 *  https://github.com/witnessmenow/arduino-twitter-api            * 
 *                                                                 *
 *  Written by Brian Lough                                         *
 *******************************************************************/
using namespace std;

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

//------- Replace the following! ------
char ssid[] = "OriginBroadband60027";       // your network SSID (name)
char password[] = "lKG8u8Rr";  // your network key

// Normally we would use these to generate the bearer token but its not working yet :/
// Use steps on the readme to generate the Bearer Token

#define BEARER_TOKEN "AAAAAAAAAAAAAAAAAAAAAMTsxAAAAAAAWfR8Q3HyobD0awyzFzB29ujQqx4%3Dtt6d8lOuQF8Ift2H59gVm0auQBm1ud6XHnjnV5bVhNeiA64pLi"

//Using curl to get bearer token
// curl -u "$CONSUMER_KEY:$CONSUMER_SECRET" \
//    --data 'grant_type=client_credentials' \
//    'https://api.twitter.com/oauth2/token'

WiFiClientSecure client;
TwitterApi api(client);

unsigned long api_mtbs = 60000; //mean time between api requests
unsigned long api_lasttime = 0;   //last time api request has been done
bool firstTime = true;

//Inputs

String screenName = "dirtywastegash";
bool haveBearerToken = false;

// Bus id definition
uint8_t bus_id[] = {0, 0, 1, 53};

// PJON object
PJONSlave<SoftwareBitBang> bus(bus_id, PJON_NOT_ASSIGNED);

int packet;
char content[] = "01234567890123456789";
bool acquired = false;
  Serial.begin(115200);

  // Set WiFi to station mode and disconnect from an AP if it was Previously
  // connected
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  // Attempt to connect to Wifi network:
  Serial.print("Connecting Wifi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  IPAddress ip = WiFi.localIP();
  Serial.println(ip);

  api.setBearerToken(BEARER_TOKEN);
  haveBearerToken = true;
  getMentions();

}
void getMentions() {
   Serial.println("Getting twitter mentions for AutoNcs");
    String responseString = api.getMentions();
    //Serial.println(responseString);
    DynamicJsonBuffer jsonBuffer;
    JsonObject& root = jsonBuffer.parseObject(responseString);
    if (root.success()){ 
      JsonArray& statuses = root["statuses"];
      for( const auto& value : statuses ) {
        JsonObject& singleStat = value;
        const char* statuses_text = singleStat["text"]; // "@AutoNcs Yo This!!"
      String theText = String(statuses_text);
      JsonObject& statuses_user = singleStat["user"];
      const char* statuses_user_screen_name = statuses_user["screen_name"]; // "dirtywastegash"
      String theUser = String(statuses_user_screen_name);  
        Serial.println("User : "+theUser+ " Said : "+theText); 
        }

      Serial.println("parsed Json");
      // Use Arduino Json to parse the data
    } else {
      Serial.println("Failed to parse Json");
    }
}
void getTwitterStats(String name) {
  Serial.println("Getting twitter stats for " + name);
    String responseString = api.getUserStatistics(name);
    Serial.println(responseString);
    DynamicJsonBuffer jsonBuffer;
    JsonObject& response = jsonBuffer.parseObject(responseString);
    if (response.success()) {
     Serial.println("parsed Json");
     //Serial.print(response);
     
    } else {
      Serial.println("Failed to parse Json");
    }
}
void receiver_handler(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info) {
  Serial.print("Received: ");
  for(uint16_t i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    Serial.print(" ");
  }
  Serial.println();
  Serial.flush();
};

void error_handler(uint8_t code, uint16_t data, void *custom_pointer) {
  if(code == PJON_CONNECTION_LOST) {
    Serial.print("Connection lost with device ");
    Serial.println((uint8_t)bus.packets[data].content[0], DEC);
  }
  if(code == PJON_ID_ACQUISITION_FAIL) {
    if(data == PJON_ID_ACQUIRE)
      Serial.println("PJONSlave error: multi-master addressing failed.");
    if(data == PJON_ID_CONFIRM)
      Serial.println("PJONSlave error: master-slave id confirmation failed.");
    if(data == PJON_ID_NEGATE)
      Serial.println("PJONSlave error: master-slave id release failed.");
    if(data == PJON_ID_REQUEST)
      Serial.println("PJONSlave error: master-slave id request failed.");
  }
  Serial.flush();
};

void setup() {
  Serial.begin(115200);
  bus.set_error(error_handler);
  bus.set_receiver(receiver_handler);
  bus.strategy.set_pin(D1);
  bus.begin();
  delay(160);
  bus.acquire_id_multi_master();
}

void loop() {
  if((bus.device_id() != PJON_NOT_ASSIGNED) && !acquired) {
    Serial.print("Acquired device id: ");
    Serial.println(bus.device_id());
    Serial.flush();
    acquired = true;
  }
  if(haveBearerToken){
    unsigned long timenow = millis();
    if (api_lasttime == 0 || timenow-api_lasttime > api_mtbs )  {
      getMentions();
      api_lasttime = timenow;
    }
  }
  bus.update();
  bus.receive(5000);
};
