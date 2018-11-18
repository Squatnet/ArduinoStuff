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

void setup() {

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
void loop() {
  delay(10);
  if(haveBearerToken){
    unsigned long timenow = millis();
    if (api_lasttime == 0 || timenow-api_lasttime > api_mtbs )  {
      getMentions();
      api_lasttime = timenow;
    }
  }
}
