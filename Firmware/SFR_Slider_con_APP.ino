/***********************************************************SFRSlider**************************************************************/
/* Hemos creado este pequeño firmware para poder hacer funcionar nuestros slider con su APP Correspondiente
 *  de momento nuestro slider es de un solo eje y se mueve de inicio a fin en funcion del tiempo indicado
 *  la APP para android que hemos creado y que podeis descargar aquí--->
 *  Para ver el video del montaje y configutacion puedes ir a mi canal de Youtube--> https://youtu.be/2Y3kqopLcV4
 *  IMPORTANTE!!! Ni en el video ni aqui se explica como configurar el modulo bluetooth, hemos seguido esta guía  --> https://cutt.ly/jjOta6g
 *  La descarga de los ficheros STL para el montaje está Aquí -- >https://github.com/Srferrete/SFRSlider/tree/main/Stls
 *  Todo esto se ha creado bajo licencia https://creativecommons.org/licenses/by-nc/4.0/ compartir y dar reconocimiento a los creadores no cuesta nada, COMPARTE Y DISFRUTA
/***********************************************************SFRSlider**************************************************************/

/*LIBRERÍAS*/
#include <SoftwareSerial.h> // Librería que nos permite configurar un nuevo modulo de conexión
#include <AccelStepper.h> // Librería para el control de motores
/*FIN LIBRERIAS*/

/*DEFINICIONES*/
#define dirPin 6 //Pines para el motor
#define stepPin 3
#define enablePin 8
#define FCpin 11 //Pin para el final de carrera
#define motorInterfaceType 1 //Indicamos a la librería accelstepper que estamos usando un driver

#define STEPS 51950 // son los pasos totatel que tiene que dar el motor para completar el recorrido.

/*CALCULO DE PASOS DE TU SLIDER*/ /*IMPORTANTE*/
//Lo primero es medir el recorrido de nuestro eje, para un perfil de 750mm se quedan habiles 650mm de recorrido.
// Nuestro motor es de 200 pasos, con el microstep a 1/16 nos deja 3200 posos para cada vuelta del motor
// Nuestra polea es Gt2 de 20 dientes, 20 x 2mm (separacion entre dientes) nos da como resultado que nuestro eje se va a mover 40mm en cada vuelta
// si nuestro recorrido es 650mm divido 40mmm, nos da el numero de vueltas que tiene que dar el motor para el recorrido completo es decir 16.25 vueltas
// multiplicamos el nuemero de vueltas x los pasos, es decir, 16.25 * 3200 = 52000 pasos, obtenemos el numero total de pasos que tiene que dar el motor  para completar el recorrido.
// ponemos algun paso menos, para evitar que el carro choque al final. en este caso 51950
/*FIN CALCULO DE PASOS*/

SoftwareSerial BT(4,2); // definimos un nuevo serial y le asignamos los pines (RX,TX)
AccelStepper stepper = AccelStepper(motorInterfaceType, stepPin, dirPin); // Creamos una instancia de AccerStepper
/*FIN DEFINICIONES*/

/*VARIABLES*/
int val; // variable para el control de los pulsadores de la APP
int horas; // variables para el tiempo
int minutos;
int segundos;
int Ttotal; // Tiempo total pasado a segundos.
long int Vel = 0; // velocidad del slider, se expresa en pasos por segundo.
/*FIN VARIABLES*/

/*SETUP*/
void setup() {
  //Serial.begin(9600); // Inicializamos la comunicacion serie por USB
  BT.begin(9600); // Inicializamos la comunicacion serie por Bluetooth
  /*setup Motor*/
  pinMode(FCpin, OUTPUT); // Configuramos los pines.
  pinMode(enablePin, OUTPUT);
  digitalWrite(FCpin, HIGH);
  stepper.setMaxSpeed(4000); // seteamos la velocidad maxima de nuestro slider.

  homing(); // mandamos el carro al punto de origen
}
/*FIN SETUP*/

/*LOOP*/
void loop() {

    while (BT.available()>0)
    {
        val = BT.parseInt(); // Metemos cada valor de la APP en una variable y lo transformamos a entero
        horas = BT.parseInt(); 
        minutos = BT.parseInt(); 
        segundos = BT.parseInt(); 
 
        //Cuando lea el carácter fin de línea ('\n') quiere decir que ha finalizado el envío de los tres valores y si se recibe un 1, hemos pulsado el boton estar y comienza el recorrido
        if (BT.read() == '\n'&& val == 1  )
        {
          Ttotal= (horas*3600)+(minutos*60)+segundos; //pasamos todo a segundos
          Vel = STEPS/Ttotal; //Calculamos la velocidad que se expresa en pasos por segundos 
          //Serial.println(horas); // para debug
          //Serial.println(minutos);
          //Serial.println(segundos);
          //delay(500);
         // Serial.println(Ttotal);
          //val=5;
          slider();
        }
        
        if(val == 0){
          //homing();
          //Serial.println("HAS PULSADO CANCEL");
        }
       
    }
        
}
/*FIN DEL LOOP*/

/*FUNCIONES*/
void homing(){
    while(digitalRead(FCpin) == HIGH){ // Mientras el final de carrera no esté pulsado va a mover el motor hacie el punto de inicio.
    digitalWrite(enablePin,LOW); 
    stepper.setSpeed(-1500);
    stepper.runSpeed();
   }
   digitalWrite(enablePin,HIGH);
   stepper.setCurrentPosition(0); // después de hacer homing indicamos que el slider está en la posición 0
   val=0;
   horas,minutos,segundos = 0; //ponemos todo a 0 y estamos listo para comezar el recorrido
   //Serial.println(val);
   //delay(5000);
}

void slider(){
  digitalWrite(enablePin,LOW);
  while(stepper.currentPosition() != STEPS){ // va a mover el motor hasta que se completen todos los pasos
    stepper.setSpeed(Vel); // seteamos la velocidad en funcion de los valores introducidos en la app
    stepper.runSpeed(); // arrancamos el recorrido
    //Serial.println(stepper.currentPosition());
    if (BT.read() == '0' ){ // si pulsamos el boton OFF de la app interrumpe el recorrido
      //homing();
      break;
    }
  }
  horas,minutos,segundos = 0; //ponemos todo a 0
  homing(); // llamamos a la funcion homing para que el slider vuelva al principio 
}
/*FIN FUNCIONES*/
