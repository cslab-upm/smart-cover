#ifndef COORDINATOR_H
#define COORDINATOR_H
#include <ArduinoJson.h>
#include "Fotodiodo.h"
#include "Servomotor.h"
#include "WiFi_MQTT.h"
#include "Medidor_de_voltaje.h"
#include <Dusk2Dawn.h>
#include <string>

#define PHOTODIODE_MAX_VALUE 400 //4000
#define SECRET_KEY "secretKey"

//int FCA = 25;
//int FCC = 32;

class Coordinador {

	private:
	/* WiFi */
      const char* ssid = "MiFibra-B7F6";
      const char* password = "tC4Rxpq3";
  /* MQTT */
	    const char* mqttServer = "m20.cloudmqtt.com";
	    const int   mqttPort = 12834;
	    const char* mqttUser = "rmqewpne";
	    const char* mqttPassword = "kFlJMJ_jC5pk";
    	char *key;

		//Variable temporizador
		int current_time;
		int last_time;
     
		/* System Components */
		Fotodiodo fotodiodo;
		Servomotor servo; 
		Medidor_de_voltaje medidorvoltaje;
		WiFi_MQTT client;
    Dusk2Dawn montegancedo;
    
	public:
      int montSunset, montSunrise;
	    void init()
	    {
	    /* Initialites MQTT Client & WiFi */
	      	client.init(ssid, password, mqttServer, mqttPort, mqttUser, mqttPassword);
	    /* Initialites System components*/
	      	servo.setup();
	      	fotodiodo.set_pin(36);
          medidorvoltaje.set_pin(34);
		  /* Initialites HMAC Key */
		    	key = SECRET_KEY; 
          montSunrise = 0;
          montSunset = 0;
          montegancedo.init(40.4165,-3.70255,1.0); //Establecemos la longitud, la latitud y la zona horaria de montegancedo (GMT +1)
		     	Serial.println("Cooridnator initialized!");

          pinMode(FCA, INPUT_PULLDOWN);
          pinMode(FCC, INPUT_PULLDOWN);
	    }
    	
//Función que permite coordinar el funcionamiento automático del mecanismo
    
    void funcionamientoAutomatico(){
        //Actualización constante del servidor de tiempo
        client.loop();
       int myear = client.my_year();  //Obtenemos el año p.ej 2020
       int mmonth = client.my_month(); //Obtenemos el mes p. ej 4
       int mday = client.my_day();  //Obtenemos el día del mes p.ej 23
       
       if (sun_light() ){
          montSunrise  = (60 * montegancedo.sunrise(myear, mmonth, mday, true));  //Le pasamos como parámetros año, mes, dia y DST (Day Saving Light): true si es horario de verano y false si es horario de invierno
          montSunset   = (60 * montegancedo.sunset(myear, mmonth, mday, true));   //de esta forma, si DST=true, se sumará una hora más, es decir GMT +2.
       }
       else {
          montSunrise  = (60 * montegancedo.sunrise(myear, mmonth, mday, false)); //Nos devuelve el tiempo del amanecer en minutos (p.ej 418). Multiplicamos por 60 para obtener los segundos.
          montSunset   = (60 * montegancedo.sunset(myear, mmonth, mday, false)); //Nos devuelve el tiempo del anochecer en minutos (p.ej 1005). Multiplicamos por 60 para obtener los segundos.
       }
       
//Se revisan las condiciones de los sensores para abrir o cerrar

      int valor_fca = digitalRead(FCA);
      int valor_fcc = digitalRead(FCC);
      
        if(medidorvoltaje.get_value()>0)
        {
            //if (client.get_time_in_seconds()<montSunrise || client.get_time_in_seconds()> montSunset){
              if (fotodiodo.get_value() > PHOTODIODE_MAX_VALUE && valor_fca == 0)
              {
                  servo.open();
              }
            //}
          if (fotodiodo.get_value() < PHOTODIODE_MAX_VALUE && valor_fcc == 0)
          {
              servo.close();   
          }
          Serial.println(" LUX= " + String(fotodiodo.get_value()) + "  " + " Voltaje= " + String(medidorvoltaje.get_value()));
        }
        else
        if (valor_fcc == 0);
        {
          servo.temporizadorclose();
        }
    }

//Con sun_light, obtenemos una aproximación de los meses en los que se cambiará la hora,
//como cada año es diferente, pero siempre es a finales de octubre y a finales de marzo,
//se decidió que para los meses abril-octubre fuese horario de verano y para el resto horario de invierno.
//Horario de verano Marzo-Noviembre (GMT +2)

   bool sun_light()
   {
      if (client.my_month()>3 && client.my_month()<11)
      {  
        return true;
      }
      else 
        return false; //GMT +1)
    }
};

#endif
