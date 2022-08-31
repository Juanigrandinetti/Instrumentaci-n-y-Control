/* 
 *  El modo de operación permite elegir si el sistema será de lazo abierto o cerrado, es decir, manual o automático, respectivamente.
 *  Modo = 1: lazo abierto -> modo manual.
 *  Modo = 2: lazo cerrado -> modo automático.
*/
int Modo = 2; 
/*
 * Definimos el rango de valores de potencia que podrá emitir el foco. En este caso, puede variar entre 100 valores distintos.
 */
float potencia_min = 0;
float potencia_max = 0;
/*
 * SetPoint (sp) se usa para elegir la temperatura deseada.
 * La temperatura final será el sp más la temperatura ambiente donde se esté realizando el sensado.
*/
float SetPoint = 48;
/*Parámetros PI*/
float Kc = 5, Ti = 5;
/*Retardo en milisegundos desde que se compila el programa hasta que la señal de entrada se pone en alto*/
int Tiempo0 = 15000;
/*PINES
 * LM35: A0 -> Sensor de temperatura.
 * PWM: 3   ->Señal que regula la potencia que emite el foco.
 * ZC: 8    ->Señal que envía el módulo dimmer cada vez que la señal de AC cruza por el origen de coordenadas.
*/
const int LM35 = A0;
const int PWM = 3;
const int ZC = 8;
/*Señal de entrada. Es la potencia enviada al foco.*/
float Potencia = 0;
/*Variables usadas para la rutina de Servicio de Interrupción.*/
int estado = 0, ZC_flag = 0;
/*Variable usada para almacenar el mapeo de los valores de potencia que emite el foco.*/
int Potencia_map = 0;
/*Variables de tiempo usadas para verificar si finalizó el retardo establecido por "Tiempo0".*/
unsigned long Tiempo_previo = 0, Tiempo_actual = 0;
/*Frecuencia de muestreo en milisegundos.*/
int fm = 1000;
/*Variable utilizada para almacenar los valores sensados por el LM35.*/
int Temperatura_LM35 = 0;
float sp = 0;
/*ERROR
 * error: diferencia entre la señal de entrada y la señal realimentada.
 * error_i: error integral.
 * error_total: suma del error proporcional más el error integral.
*/
float error = 0, error_i = 0, error_total = 0;
void setup() {
  Serial.begin(9600);
  pinMode(PWM, OUTPUT);
  pinMode(ZC, INPUT);
  /*Habilitamos las interrupciones del puerto B.*/
  PCICR |= B00000001;
  /*Habilitamos el pin 8 ó D0 para interrupciones.*/
  PCMSK0 |= B00000001;
}

void loop() {
  Tiempo_actual = millis();
  Potencia_map = map(Potencia, 0, 100, 0, 2000);
  if(ZC_flag)
  {
    delayMicroseconds(Potencia_map);
    digitalWrite(PWM, HIGH);
    delayMicroseconds(100);
    digitalWrite(PWM, LOW);
    ZC_flag = 0;
    /*Verificamos que haya pasado 1 segundo para así, poder tomar una muestra.*/
    if(Tiempo_actual - Tiempo_previo >= fm)
    {
      Tiempo_previo += fm;
      Temperatura_LM35 = 100.0*(5.0*analogRead(LM35)/1024.0 - .5);
      if(Modo == 1)
      {
        if(Tiempo_actual <= Tiempo0) Potencia = potencia_min;
        else if(Tiempo_actual >= Tiempo0) Potencia = potencia_max;
      }
      else if(Modo == 2)
      {
        if(Tiempo_actual <= Tiempo0) error_total = 0;
        else if(Tiempo_actual >= Tiempo0)
        {
          error = SetPoint - Temperatura_LM35;
          error_i = error_i + error*(1000/fm);
          error_total = Kc*(error + (1/Ti)*error_i);
        }
        /*Valores límites para la salida del controlador.*/
        if(error_total < 0) error_total = 0;
        if(error_total > 100) error_total = 100;
        /*Asignamos el error a la entrada del actuador para que implemente la acción correctora que corresponda.*/
        Potencia = error_total;
        sp = SetPoint;
      }
      Serial.print(Potencia);
      Serial.print(" ");
      Serial.print(Temperatura_LM35);
      Serial.print(" ");
      Serial.println(sp);
    }
  }
}
/*Rutina de Servicio de Interrupción*/
ISR(PCINT0_vect)
{
  /*Verificamos que el puerto B y el pin 8 ó D0 tengan las interrupciones habilitadas.*/
  if(PINB & B00000001)
  {
    if(estado == 0) 
    {
      ZC_flag = 1;
    }
  }
  else if(estado == 1) 
  {
    ZC_flag = 1;
    estado = 0;
  }
}
