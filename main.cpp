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
// Drivetrain           drivetrain    19, 20, 1       
// Distance2            distance      2               
// ---- END VEXCODE CONFIGURED DEVICES ----

#include "vex.h"

using namespace vex;

// Numero de mm del tramo.
float rectas[] = {500,500,500,500}; // 1000mm = 100 cm = 1m
// Valores boleanos, True = Giro Derecha III False = Giro izquierda  (Giros de 90º)
bool curvas[] ={1,1,1,1}; 
//Numero de rectas
int num_rect = sizeof(rectas);
int num_curv = sizeof(curvas);
// Variable temporal control recorrido total (linea recta)
int acumulador = 0;
// Control de casos línite
int dist_por_recorrer = 0;

int main() 
{
  // Initializing Robot Configuration. DO NOT REMOVE!
  vexcodeInit();
  
  for(int i = num_rect; i > 0; i--)
  {
    // reset variable temporal control recorrido total (linea recta)
    acumulador = 0;

    // Avanza 
    while(acumulador < rectas[i])
    {
      // Variable que almacena cuanto falta por recorrer
      dist_por_recorrer = rectas[i] - acumulador;

      // Si la distancia es superior a un giro de rueda, gura una rueda
      if (dist_por_recorrer > 100)
      {
        Drivetrain.driveFor(forward, 100, mm);
        acumulador += 100;
      }
      else // Si la dit es menor a un giro de rueda, gura lo que falte
      {
        Drivetrain.driveFor(forward, dist_por_recorrer, mm);
        break;
      }

      // Si detecta un objecte
      if(Distance2.isObjectDetected())
      {
        // Si l'objecte està a prop
        if(Distance2.objectDistance(mm) < 300)
        {
          // Inicia sequancia rodeo
          Drivetrain.setRotation(90, degrees); // Gira derecha
          Drivetrain.driveFor(forward, 750, mm); // Lateraliza hacia la derecha
          Drivetrain.setRotation(-90, degrees); // Gira izquierda
          Drivetrain.driveFor(forward, 900, mm); // Linea recta 
          // AQUI HAY UN CASO LÍMITE POR VALORAR
          Drivetrain.setRotation(-90, degrees); // Gira izquierda
          Drivetrain.driveFor(forward, 750, mm); // Lateraliza hacia la izquierda
          Drivetrain.setRotation(90, degrees); // Gira derecha

          // Actualiza distancia recorrida
          acumulador += 900;
        }
      }
    }

    printf("Recta"); 

    // Gira
    if(i-1 > 0)
    {     
      if (curvas[1] == 1)
      {
        Drivetrain.setRotation(90, degrees);
        printf("Giro derecha"); 
      }
      else
      {
        Drivetrain.setRotation(-90, degrees);
        printf("Giro izquierda"); 
      }
    }
  }
}