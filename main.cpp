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

// Numero de mm del tramo.
float rectas[] = {(800+220), (1600-220), (800-220)}; // 1000mm = 100 cm = 1m Bravo eric
// Valores boleanos, True = Giro Derecha III False = Giro izquierda  (Giros de 90º)
bool curvas[] ={1,1}; 
//Numero de rectas
int num_rect = sizeof(rectas);
int num_curv = sizeof(curvas);
// Variable temporal control recorrido total (linea recta)
int acumulador = 0;
// Control de casos línite
int dist_por_recorrer = 0;

int main() 
{
  vexcodeInit();
  Drivetrain.setHeading(0, degrees);
  wait(3,sec);

  for(int i = 0; i < (num_rect - 1) ; i++)
  {
    // reset variable temporal control recorrido total (linea recta)
    acumulador = 0;

    // Avanza 
    while(acumulador < rectas[i])
    {

      // Variable que almacena cuanto falta por recorrer
      dist_por_recorrer = rectas[i] - acumulador;

      // Si la distancia es superior o igual a un giro de rueda, gira una rueda
      if (dist_por_recorrer >= 100)
      {
        Drivetrain.driveFor(reverse, 100, mm);
        acumulador += 100;
      }
      else // Si la dist es menor a un giro de rueda, gura lo que falte
      {
        Drivetrain.driveFor(reverse, dist_por_recorrer, mm);
        break;
      } 

      // Si detecta un objecte
      if(Distance11.isObjectDetected())
      {
        wait(0.3, sec); // Bug
        // Si l'objecte està a prop
        if(Distance11.objectDistance(mm) < 250 && Distance11.objectDistance(mm) > 100)
        {

          //Inicia sequancia rodeo
          Motor20.spin(forward);
          Drivetrain.turnToHeading(90,degrees);; // Gira derecha
          Drivetrain.setHeading(0,degrees);
          Drivetrain.driveFor(reverse, 700, mm); // Lateraliza hacia la derecha
          Drivetrain.turnToHeading(-90,degrees);; // Gira izquierda
          Drivetrain.setHeading(0,degrees);
          Drivetrain.driveFor(reverse, 1100, mm); // Linea recta 
          // AQUI HAY UN CASO LÍMITE POR VALORAR
          Drivetrain.turnToHeading(-90,degrees);; // Gira izquierda
          Drivetrain.setHeading(0,degrees);
          Drivetrain.driveFor(reverse, 700, mm); // Lateraliza hacia la izquierda
          Drivetrain.turnToHeading(90,degrees);; // Gira derecha
          Drivetrain.setHeading(0,degrees);
          Motor20.stop();

          // Actualiza distancia recorrida
          acumulador += 1000;
          
        }
      }
    }

    // CAS LÍMIT CUB EN UNA PUNTA
    if(acumulador > rectas[i])
    {
      Drivetrain.driveFor(forward, (acumulador - rectas[i]), mm);
    }

    // Gira
    if(i < num_curv)
    {     
      if (curvas[i] == 1)
      {
        Drivetrain.turnToHeading(90,degrees);
      }
      
      if(curvas[1] == 0)
      {
        Drivetrain.turnToHeading(-90,degrees); // Gira izquierda
      }
    }
    Drivetrain.setHeading(0,degrees);
    wait(1,sec);
  }

  // Por un bug
  while(true)
  {
    Motor20.spin(reverse);
    Drivetrain.stop();
  }
  
  wait(100,msec);
}