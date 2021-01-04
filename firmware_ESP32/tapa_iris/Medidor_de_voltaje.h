#include "Sensor.h"

class Medidor_de_voltaje: public Sensor{

	public:
		Medidor_de_voltaje(int pin1){
			pin=pin1;
		}
    Medidor_de_voltaje(){
      
    }
		int get_value(){
			value=analogRead(pin);
			return (value);
		}
};
