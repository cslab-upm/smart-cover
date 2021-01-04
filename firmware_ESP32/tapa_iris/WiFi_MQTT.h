#include <NTPClient.h>
#include <WiFi.h>
#include "HmacSha256.h"
#include <PubSubClient.h>
#include <WiFiUdp.h>
#define MQTT_MAX_PACKET_SIZE 1024

class WiFi_MQTT {
	
  public:
	WiFiClient wifiClient;
	PubSubClient MQTTClient;  

	char mensaje_inicial[100];
	int flag_msg_recibido = 0;
	
	WiFiUDP ntpUDP;
	NTPClient timeClient;

    void init(const char* ssidd, const char* pswd, const char* mqtt_Server ,const int mqtt_Port ,const char* mqtt_Usr ,const char* mqtt_Pswd ){
  		timeClient.setUDPClient(ntpUDP);
      //Fijar zona horaria española
      timeClient.setTimeOffset(3600);
  		MQTTClient.setClient(wifiClient);
  
  		WiFi.begin(ssidd, pswd);
  
  		while (WiFi.status() != WL_CONNECTED) {
  			delay(500);
  			Serial.println("Connecting to WiFi..");
  		}
  
  		Serial.println("Connected to the WiFi network");
  
  		MQTTClient.setServer(mqtt_Server, mqtt_Port);
  
  /*
  		while (!MQTTClient.connected()) {
  
    			Serial.println("Connecting to MQTT...");
    
    			if (MQTTClient.connect("ESP32Client", mqtt_Usr, mqtt_Pswd,"esp/LastWill",0,1,"0")) {
    
    				Serial.println("connected"); 
    				//client.publish("esp/test1", "ESP32 conectado !"); 
    			} 
    			else { 
    
    				Serial.print("failed with state ");
    				Serial.print(MQTTClient.state());
    				delay(2000);
    			}
  		  } */
    }

    String get_time(){
    	timeClient.update();

    	char msgToSend[10];
		sprintf(msgToSend, "%02d:%02d:%02d", timeClient.getHours(), timeClient.getMinutes(), timeClient.getSeconds());
		delay(100);
		String str1(msgToSend);
		return /*static_cast<char*>(*/str1/*)*/;

    }

    int get_time_in_seconds(){
    	int total = 0;
    	total = timeClient.getHours()*3600 + timeClient.getMinutes()*60 + timeClient.getSeconds();  //Obtiene hora:minuto:segundo del día
      return total;
    }

    uint8_t my_month(){
      unsigned long secs = 0;
      uint8_t month;
      secs = timeClient.getEpochTime();
      month = timeClient.getMonth(secs);
      return month;
    }

    uint8_t my_year(){
        uint8_t year;
        year = timeClient.getYear();

        return year;
    }

    uint8_t my_day(){
        uint8_t day;
        day = timeClient.getDate();

        return day;
    }

    void loop() {
      while(!timeClient.update()) {
      timeClient.forceUpdate();
      }
    }

};
    
