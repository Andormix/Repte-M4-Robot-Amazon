/*----------------------------------------------------------------------------*/
/*                                                                            */
/*    Module:       main.cpp                                                  */
/*    Author:       C:\Users\Andormix                                         */
/*    Created:      Tue Jun 13 2023                                           */
/*    Description:  V5 project                                                */
/*                                                                            */
/*----------------------------------------------------------------------------*/

// ---- START VEXCODE CONFIGURED DEVICES ----
// Robot Configuration:
// [Name]               [Type]        [Port(s)]
// Drivetrain           drivetrain    17, 18, 1       
// Distance11           distance      11              
// Motor20              motor         20              
// Vision9              vision        9               
// ---- END VEXCODE CONFIGURED DEVICES ----

#include "vex.h"

using namespace vex;

// Para que quede centrado desde la pos inicial
const int offset = 260; 

// Numero de mm del tramo (1000mm = 100 cm = 1m )
float rectas[] = {(2000 + offset)}; 

// True (1) = Giro Derecha and False = Giro izquierda  (Giros de 90º) 
bool curvas[] ={};  

// Numero de rectas y curvas por hacer
int num_rect = sizeof(rectas) / sizeof(int);
int num_curv = sizeof(curvas) / sizeof(bool);

// Variable temporal control recorrido total (linea recta)
int acumulador = 0;

// Control de casos línite
int dist_por_recorrer = 0;

int main() 
{
  vexcodeInit();
  Drivetrain.setHeading(0, degrees);
  wait(1,sec);

  // Testing
  printf("tenemos %i rectas por hacer.\n", num_rect);
  printf("tenemos %i curvas por hacer.\n", num_curv);
  
  for(int i = 0; i < (num_rect); i++)
  {
    // reset variable temporal control recorrido total (linea recta)
    acumulador = 0;

    // Testing
    printf("Recta no. %i\n" , i);

    // AVANÇAR
    while(acumulador < rectas[i])
    {
      // Variable que almacena cuanto falta por recorrer
      dist_por_recorrer = rectas[i] - acumulador;

      // Si la distancia es superior o igual a un giro de rueda, gira una rueda
      if (dist_por_recorrer >= 100)
      {
        Drivetrain.driveFor(reverse, 100, mm);
        Drivetrain.turnToHeading(0,degrees); // Corrige desviación
        acumulador += 100;
      }
      else // Si la dist es menor a un giro de rueda, gira lo que falte
      {
        Drivetrain.driveFor(reverse, dist_por_recorrer, mm);
        Drivetrain.turnToHeading(0,degrees); // Corrige desviación
        break;
      } 

      // Si detecta un objecte
      if(Distance11.isObjectDetected())
      {
        // Si l'objecte està a prop
        if(Distance11.objectDistance(mm) < 250 && Distance11.objectDistance(mm) > 100)
        {
          //Inicia sequancia rodeo
          Motor20.spin(forward);
          Drivetrain.turnToHeading(90,degrees); // Gira derecha
          Drivetrain.setHeading(0,degrees);

          Drivetrain.driveFor(reverse, 700, mm); // Lateraliza hacia la derecha
          Drivetrain.turnToHeading(0,degrees); // Corrige desviación

          Drivetrain.turnToHeading(-90,degrees);; // Gira izquierda
          Drivetrain.setHeading(0,degrees);

          Drivetrain.driveFor(reverse, 1100, mm); // Linea recta 
          Drivetrain.turnToHeading(0,degrees); // Corrige desviación

          Drivetrain.turnToHeading(-90,degrees);; // Gira izquierda
          Drivetrain.setHeading(0,degrees);

          Drivetrain.driveFor(reverse, 700, mm); // Lateraliza hacia la izquierda
          Drivetrain.turnToHeading(0,degrees); // Corrige desviación

          Drivetrain.turnToHeading(90,degrees);; // Gira derecha
          Drivetrain.setHeading(0,degrees);
          Motor20.stop();

          // Actualiza distancia recorrida
          acumulador += 1100;
               
        }
      }
    }

    // CORREGIR (CAS LÍMIT CUB EN UNA PUNTA DE LA RECTA)
    if(acumulador > rectas[i])
    {
      // Retrocedeix la distància que ha fet de més-
      Drivetrain.driveFor(forward, (acumulador - rectas[i]), mm);
    }

    // GIRAR
    if(i < num_curv)
    { 
      if (curvas[i] == 1)
      {
        Drivetrain.turnToHeading(90,degrees);
      }
      
      if(curvas[i] == 0)
      {
        Drivetrain.turnToHeading(-90,degrees); // Gira izquierda
      }

      Drivetrain.setHeading(0,degrees);
    }

    wait(0.3,sec);
  }

  Drivetrain.stop();
  Motor20.spin(reverse);
  wait(40,sec);
  Motor20.stop();
}