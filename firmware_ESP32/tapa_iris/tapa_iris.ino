#include "Coordinador.h"
Coordinador coordinador;
int num = 0;
void setup()
{
  Serial.begin(115200);
  coordinador.init();
}
void loop()
{
	coordinador.funcionamientoAutomatico();
 delay(1000);
}
