#include <ssl_client.h>
#include <WiFiClientSecure.h>
#include <MyTime.h>
#include "Final_de_carrera.h"
#include <ESP32Servo.h>

#define PHOTODIODE_MAX_VALUE 400 //4000

#define LEDC_CHANNEL_0     0    // use first channel of 16 channels (started from zero)
#define LEDC_CHANNEL_1     1
#define FwdPin1            12
#define RevPin2            27
#define LEDC_TIMER_8_BIT   8    // use 8 bit precission for LEDC timer
#define LEDC_BASE_FREQ     5000 // use 5000 Hz as a LEDC base frequency

int OutFwdVal = 0;
int OutRevVal = 0;

int FCA = 25;
int FCC = 32;
int PWM = 255;

class Servomotor: public Sensor{

	public:

		Final_de_carrera sensor_apertura;
		Final_de_carrera sensor_cierre;
    Fotodiodo mifotodiodo;
    //MPU_6050 mpu_sensor;
		//Servo myservo; 
    hw_timer_t *watchdogTimer1 = NULL;

    
    double tiempo1 = 0;
    //const String slack_hook_url = "https://hooks.slack.com/services/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";
    const String slack_hook_url = "https://hooks.slack.com/services/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"; //Hugo
    
    const String slack_message = "Error durante la apertura IRIS";
    
    const String slack_message_sensor_apertura = "Sensor final de carrera de apertura no comunica IRIS";
    const String slack_message_sensor_cierre = "Sensor final de carrera de cierre no comunica IRIS";

    const String slack_message_tapa_apertura = "La tapa se ha ABIERTO correctamente IRIS";
    const String slack_message_tapa_cierre = "La tapa se ha CERRADO correctamente IRIS";
    
    const String slack_username = "Hugo Albarracin Meneses";
   
    
		void set_pin(int x){
			pin = x;
			sensor_apertura.set_pin(FCA); //5
			sensor_cierre.set_pin(FCC); //4

		}

		void setup(){
  			//myservo.setPeriodHertz(50);// Standard 50hz servo
  			//myservo.attach(pin, 500, 2500);
  			watchdogTimer1 = timerBegin(0,80,true);
        value = 0;
        
        ledcSetup(LEDC_CHANNEL_0, LEDC_BASE_FREQ, LEDC_TIMER_8_BIT); // Setup timer and attach timer to a led pin
        ledcAttachPin(FwdPin1, LEDC_CHANNEL_0);
        ledcSetup(LEDC_CHANNEL_1, LEDC_BASE_FREQ, LEDC_TIMER_8_BIT); // Setup timer and attach timer to a led pin
        ledcAttachPin(RevPin2, LEDC_CHANNEL_1);
        analogSetWidth(10); //la lectura analogica se setea de 0 a 1024
        
        pinMode(FCA, INPUT_PULLDOWN);
        pinMode(FCC, INPUT_PULLDOWN);
        

 		}
	  int get_value(){
		
			return(value);
		}
		
		bool open()
		{
      Serial.println("Preparando para abrir.");
      int valor_fca = digitalRead(FCA);
      int valor_fcc = digitalRead(FCC);
      int flag = 0;
      Serial.println("FCA= " + String(valor_fca) + "  "  + "FCC= " + String(valor_fcc));
      OutFwdVal = PWM;
      OutRevVal = 0;
			if(valor_fca == 0)
			{
        Serial.println("Abriendo tapa...");  
        timerWrite(watchdogTimer1, 0); // reset timer feed dog
        timerStart(watchdogTimer1);
        tiempo1 = timerReadSeconds(watchdogTimer1);
  			while(valor_fca==0 && flag==0 )
  			{
          valor_fca = digitalRead(FCA);
          ledcWrite(LEDC_CHANNEL_0,OutFwdVal);
          ledcWrite(LEDC_CHANNEL_1,OutRevVal);
          if (mifotodiodo.get_value()> PHOTODIODE_MAX_VALUE)
          {
              flag = 1;
          }
          //Serial.println("Luz= " + String(mifotodiodo.get_value()));
        }
        if (tiempo1 > 8)
    		{
          //bool error = postMessageToSlack(slack_message);
          //bool error = postMessageToSlack(slack_message_sensor_apertura); //Sensor de final de carrera de apertura no comunica
          Serial.print("Error durante la apertura IRIS");
          Serial.print("Sensor de final de carrera de apertura no comunica IRIS");
          tiempo1 = timerReadSeconds(watchdogTimer1);
          temporizadorclose();
          timerStop(watchdogTimer1);
          timerWrite(watchdogTimer1,0);
       	}
			}

     if (valor_fca == 1)
     {
        OutFwdVal = 0;
        OutRevVal = 0;
        ledcWrite(LEDC_CHANNEL_0,OutFwdVal);
        ledcWrite(LEDC_CHANNEL_1,OutRevVal);
        //bool tapa = postMessageToSlack(slack_message_tapa_apertura); // LA TAPA SE HA ABIERTO CORRECTAMENTE.
        Serial.print("LA TAPA SE HA ABIERTO CORRECTAMENTE  IRIS");
     }
    Serial.println(" Fwd= " + String(OutFwdVal) + "  " + "Rev= " + String(OutRevVal) + "  "  + "FCA= " + String(valor_fca) + "  "  + "FCC= " + String(valor_fcc));
      
       return true;  
      timerStop(watchdogTimer1);
			timerWrite(watchdogTimer1,0);
		}
			
		bool close()
		{
    Serial.println("Preparando para cerrar.");
      int valor_fca = digitalRead(FCA);
      int valor_fcc = digitalRead(FCC);
      int flag = 0;
      OutFwdVal = 0;
      OutRevVal = PWM;
      Serial.println("FCA= " + String(valor_fca) + "  "  + "FCC= " + String(valor_fcc));
      if(valor_fcc == 0)
			{
			Serial.println("Cerrando tapa...");
				while(valor_fcc == 0 && flag == 0 )
				{
				  valor_fcc = digitalRead(FCC);
          ledcWrite(LEDC_CHANNEL_0,OutFwdVal);
          ledcWrite(LEDC_CHANNEL_1,OutRevVal);
				}
				if (valor_fcc == 0 && value == 156){
          //bool error = postMessageToSlack(slack_message_sensor_cierre); //Sensor de final de carrera de cierre no comunica.
          Serial.print("Sensor de final de carrera de cierre no comunica IRIS");
				}    
			}
      if (valor_fcc == 1)
      {
        OutFwdVal = 0;
        OutRevVal = 0;
        ledcWrite(LEDC_CHANNEL_0,OutFwdVal);
        ledcWrite(LEDC_CHANNEL_1,OutRevVal);
        //bool tapa = postMessageToSlack(slack_message_tapa_cierre); // LA TAPA SE HA CERRADO CORRECTAMENTE.
        Serial.print("LA TAPA SE HA CERRADO CORRECTAMENTE IRIS");
      }
      Serial.println(" Fwd= " + String(OutFwdVal) + "  " + "Rev= " + String(OutRevVal)  + "  "  + "FCA= " + String(valor_fca) + "  "  + "FCC= " + String(valor_fcc));
     
    return true;
		}

   void temporizadorclose()
   {
      int valor_fcc = digitalRead(FCC);
      timerWrite(watchdogTimer1,0);
      if(valor_fcc == 0)
      {
        OutFwdVal = 0;
        OutRevVal = PWM;
        Serial.println("Cerrando tapa por emergencia...");
        while(valor_fcc == 0)
        {
          valor_fcc = digitalRead(FCC);
          ledcWrite(LEDC_CHANNEL_0,OutFwdVal);
          ledcWrite(LEDC_CHANNEL_1,OutRevVal);
        }
      }
      OutFwdVal = 0;
      OutRevVal = 0;
      ledcWrite(LEDC_CHANNEL_0,OutFwdVal);
      ledcWrite(LEDC_CHANNEL_1,OutRevVal);
      Serial.println(" Fwd= " + String(OutFwdVal) + "  " + "Rev= " + String(OutRevVal)  + "  "  + "FCC= " + String(valor_fcc));
      Serial.print("CERRADO DE EMERGENCIA");
   }
   
   bool postMessageToSlack(String msg){
      const char* host = "hooks.slack.com";
      Serial.print("Connecting to ");
      Serial.println(host);
    
      // Use WiFiClient class to create TCP connections
      WiFiClientSecure client;
      const int httpsPort = 443;
      if (!client.connect(host, httpsPort)) {
        Serial.println("Connection failed :-(");
        return false;
      }   
      Serial.print("Posting to URL: ");
      Serial.println(slack_hook_url);
      String postData="payload={\"username\": \"" + slack_username + "\", \"text\": \"" + msg + "\"}";
      client.print(String("POST ") + slack_hook_url + " HTTP/1.1\r\n" +
                   "Host: " + host + "\r\n" +
                   "Content-Type: application/x-www-form-urlencoded\r\n" +
                   "Connection: close" + "\r\n" +
                   "Content-Length:" + postData.length() + "\r\n" +
                   "\r\n" + postData);
      Serial.println("Request sent");
      String line = client.readStringUntil('\n');
      Serial.printf("Response code was: ");
      Serial.println(line);
      if (line.startsWith("HTTP/1.1 200 OK")) {
        return true;
      } else {
        return false;
      }
   }
};
