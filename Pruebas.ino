/* Suscribete al canal de Profe Misa
   https://www.youtube.com/c/ProfeMisa 

   Código para control de temperatura usando PID
   Basado en 220V AC PID CONTROL - CODE, para mas detalles consultar ELECTRONOOBS en Español  https://youtu.be/o3Q56a3mvOg

   Para este prototipo, el objetivo de control es mantener la temperatura del foco en un setpoint   
   Variable de entrada = Potencia (0 - 100), Variable de salida = Temperatura (Temp Ambiente - 80 ºC)
*/ 
int Modo = 2;  // Modo de operacion 1: Modo manual (lazo abierto)
               //                   2: Modo automatico (lazo cerrado)
               
// Parametros para analisis a lazo abierto
float Potencia_1 = 0;     // valor inicial del cambio escalon. (min = 0)
float Potencia_2 = 0;   // Valor final del cambio escalon. (max = 100)

// Parametros para analisis a lazo cerrado
float Setpoint = 46;       // Celsius

// Constantes de PID
float Kc = 1; float Tao_I = 110;

int Tiempo0 = 15000;     // Retardo (en milisegundos) para ejecutar cambio escalon cuando se encuentra 
                         //a lazo abierto o cambio en el septpoint a lazo cerrado 

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////     NO TOCAR      //////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////     NO TOCAR      //////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////     NO TOCAR      //////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int A = 0;           // Pin A0 de entrada analogica para sensor LM35 (Variable de salida)
float Potencia = 0;  // Potencia inicial enviada al dimmer en rango de 0 a 100 (Variable de entrada)

//  Declaracion de variables
int pin_disparo = 3;
int pin_cruce_cero = 8;

int last_CH1_state = 0;
int detectado = 0;
int valor=0;

unsigned long Tiempo_previo = 0; 
unsigned long Tiempo_actual = 0;
int Read_Delay = 1000;     // Periodo de muestreo en milisegundos
int Temperatura = 0;       // Celsius
float sp = 0;    

// Variables para PID
float PID_error = 0;
float previous_error = 0;
float PID_value = 0;
float Error_INT = 0;

void setup() {
  Serial.begin(9600);
  //Definicion de pines
  pinMode (pin_disparo,OUTPUT); 
  pinMode (pin_cruce_cero,INPUT);
  PCICR |= B00000001;                                                     
  PCMSK0 |= B00000001;  
}

void loop() {
  
    Tiempo_actual = millis(); // Tiempo Actual    
     
    valor = map(Potencia,0,100,0,2000);
    if (detectado)
    {
      delayMicroseconds(valor);
      digitalWrite(3,HIGH);
      delayMicroseconds(100);
      digitalWrite(3,LOW);
      detectado=0;

    if(Tiempo_actual - Tiempo_previo >= Read_Delay){
       Tiempo_previo += Read_Delay;                
    
    Temperatura = 100.0*(5.0*analogRead(A)/1024.0 - .5);         //Lectura del sensor LM35

    if(Modo == 1){
      // Modo manual (lazo abierto)
      if(Tiempo_actual <= Tiempo0) {
        Potencia = Potencia_1;
      }
      else if(Tiempo_actual >= Tiempo0) {
        Potencia = Potencia_2;
      } 
      
    }
    else if(Modo == 2){
    // Modo automatico (lazo cerrado)
    if(Tiempo_actual <= Tiempo0) {
       PID_value = 0;
    }
    else if (Tiempo_actual >= Tiempo0){
      PID_error = Setpoint - Temperatura;                   //Calculo del error    
      Error_INT = Error_INT + PID_error*(1000/Read_Delay);  //Calculo de la integral del error
      PID_value = Kc*(PID_error + (1/Tao_I)*Error_INT);     //Calculo de la salida del controlador PI

      sp = Setpoint;
      
    }
    // Limite de salida del controlador
    if(PID_value < 0)
    {      PID_value = 0;       }
    if(PID_value > 100)
    {      PID_value = 100;    }

    Potencia = PID_value;   //Asignacion a la entrada de la planta.
    }    
    
        Serial.print(Potencia);
        Serial.print(" ");
        Serial.print(Temperatura);
        Serial.print(" ");
        Serial.println(sp);    
    }
    
    } 
}

//This is the interruption routine
//----------------------------------------------

ISR(PCINT0_vect){
  /////////////////////////////////////               //Input from optocoupler
  if(PINB & B00000001){                               //We make an AND with the pin state register, We verify if pin 8 is HIGH???
    if(last_CH1_state == 0){                          //If the last state was 0, then we have a state change...
      detectado=1;                                    //We haev detected a state change!
    }
  }
  else if(last_CH1_state == 1){                       //If pin 8 is LOW and the last state was HIGH then we have a state change      
    detectado=1;                                      //We haev detected a state change!
    last_CH1_state = 0;                               //Store the current state into the last state for the next loop
    }
}
