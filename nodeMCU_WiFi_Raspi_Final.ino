/* ############################################################################################
   #                                                                                          #
   #  @ Repte M4 any 2023                                                                     #
   #  @ Alumnes: Joan Fornis i Eric Torrontera                                                #
   #                                                                                          #
   ############################################################################################ */

/* ############################################################################################
   #                                   LLIBRERIES                                             #
   ############################################################################################ */

#include <ESP8266WebServer.h>
#include "DHTesp.h" // Click here to get the library: http://librarymanager/All#DHTesp
#include <Wire.h>    // I2C library
#include "ccs811.h"  // CCS811 library

/* ############################################################################################
  #                                   ERROR HANDLE                                           #
  ############################################################################################ */

#ifdef ESP32
#pragma message(THIS EXAMPLE IS FOR ESP8266 ONLY!)
#error Select ESP8266 board.
#endif

/* ############################################################################################
   #                                    DADES DE XARXA                                        #
   ############################################################################################ */

//Dades Xarxa Wifi Uni (Recordar de registrar MAC !!!)
//Servidor host que volem accedir
const char* ssid     = "Universitat d'Andorra";
const char* password = "";
const String host = "192.168.13.238"; //Adreça de la RaspBerryPi3 (Veure ServidorTest.py) // Ping RaspiR1 en CMD

/* ############################################################################################
   #                                   VARIABLES                                              #
   ############################################################################################ */

//Apartat DHT
DHTesp dht; 

//Apartat CO2 
CCS811 ccs811(0); 
float val1, val2;

//Preparar format del GET per enviar al servidor
String dada1="?Temperatura=";
String dada2="&Humetat=";
String dada3="&CO2=";
String dada4 ="&tvoc=";
String dada5 = "&Switch1=";
String dada6 = "&Switch2=";

// Polsadors (VEX, part mecanitzada)
int Switch1 = 0;
int Switch2 = 0;
int button1 = 0;
int button2 = 0;
int temporal = 0; 	 // variable temporal para el estatus del botón.

/* ############################################################################################
   #                                        SETUP                                             #
   ############################################################################################ */
 
void setup()
{
  
  /* ############################################################################################
     #                 Preprar monitor i connexió WiFi (Adreça IP)                              #
     ############################################################################################ */

  Serial.begin(115200);
  Serial.println();
  Serial.println();
  Serial.print("Connectant a ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) 
  { 
    delay(500);  
    Serial.print('.'); 
  }
  Serial.println("");
  Serial.print("Wifi OK :\t");
  Serial.println(ssid);
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP()); //Adreça que ens donarà el DHCP
  pinMode(2, OUTPUT); //
  digitalWrite(2, LOW);

  /* ############################################################################################
     #                           Preprar pins DHT ESP8266                                       #
     ############################################################################################ */

  dht.setup(14,DHTesp::DHT11); //Pin sensor conectat a D1
  pinMode(14, OUTPUT); //
  digitalWrite(14, LOW);

  /* ############################################################################################
     #                         Preprar sensor CO2 (CSS811) + I2C WIRING                         #
     ############################################################################################ */ 
  
  // Habilitat I2C
  Wire.begin(); 

  // Habilitar CCS811
  ccs811.set_i2cdelay(50); // Needed for ESP8266 because it doesn't handle I2C clock stretch correctly
  bool ok = ccs811.begin();
  if ( !ok ) Serial.println("setup: CCS811 begin FAILED");

  // Print CCS811 versions
  Serial.print("setup: hardware    version: "); Serial.println(ccs811.hardware_version(), HEX);
  Serial.print("setup: bootloader  version: "); Serial.println(ccs811.bootloader_version(), HEX);
  Serial.print("setup: application version: "); Serial.println(ccs811.application_version(), HEX);

  // Empezar a tomar mediciones
  ok = ccs811.start(CCS811_MODE_1SEC);
  if ( !ok ) Serial.println("setup: CCS811 start FAILED");
  delay(2000);

  /* ############################################################################################
     #                                   BOTONS                                                 #
     ############################################################################################ */

  pinMode(button1, INPUT); // declare push button as input
}

/* ############################################################################################
   #                                      MAIN LOOP                                           #
   ############################################################################################ */

void loop()
{
  // ######################################## LOOP DHT #######################################

  delay(dht.getMinimumSamplingPeriod());
  float hum = dht.getHumidity();
  float temp = dht.getTemperature();
  Serial.print("Temperatura: ");
  Serial.print(temp);
  Serial.print(" Humetat: ");
  Serial.print(hum);

  // #######################################  APARTAT CO2 #######################################
  
  uint16_t eco2, etvoc, errstat, raw; // Llegir
  ccs811.read(&eco2, &etvoc, &errstat, &raw);
  Serial.println();

  // Print measurement results based on status
  if ( errstat == CCS811_ERRSTAT_OK )
  {
    val1 = eco2;
    val2 = etvoc;

    Serial.print("CCS811: ");
    Serial.print("eco2=");
    Serial.print(val1);
    Serial.print(" ppm  ");

    Serial.print("etvoc=");
    Serial.print(val2);
    Serial.print(" ppb  ");
    Serial.println();
  }
  else if ( errstat == CCS811_ERRSTAT_OK_NODATA )
  {
    Serial.println("CCS811: Esperando nuevos datos");
  } else if ( errstat & CCS811_ERRSTAT_I2CFAIL )
  {
    Serial.println("CCS811: I2C error");
  }
  else
  {
    Serial.print("CCS811: errstat=");
    Serial.print(errstat, HEX);
    Serial.print("=");
    Serial.println( ccs811.errstat_str(errstat) );
  }

  // #######################################  BOTONS VEX A ARDUINO #######################################

  temporal = digitalRead(button1);
     
  if (temporal == HIGH) 
  {
    Serial.println("LED OFF");
    Switch1 = 0;
  }
  else 
  {
    Serial.println("LED ON");
    Switch1 = 1;
  }
  Serial.println();

  // ######################################## LOOP Pujar dades Servidor ####################################### 
    
  WiFiClient client; //Connexió a servidor Web Python en RaspberryPi 3 (Miki)
  const int httPort=8080;
  if (!client.connect(host,httPort)) 
  {
  Serial.println("Error en connexió Servidor caigut");
  delay(2000);
  return;
  }

  // Enviar petició GET
  String url =String("http://" + host + ":8080/dades"); //Adreça obtinguda per DHCP
  Serial.println("Format de la URL : ");
  String cade=String("GET "+url + dada1 + temp + dada2 + hum + dada3 + eco2 + dada4 + val2 + dada5 + Switch1 + dada6 + Switch2 + " HTTP/1.1\r\n" +
                "Host: " + host + "\r\n" +
                "Connection: close\r\n\r\n");
                
  Serial.print(cade); //Veure en monitor format de la Petició per debugging
  client.print(cade); //Enviar petició GET al Servidor Web
  
  //Esperar resposta del Servidor (timeout als 5 segons)
  unsigned long timeout=millis(); //Top Crono per 5 segons
  while (client.available()==0) 
  {
  if (millis()-timeout >5000) //Han transcorregut 5 segons--> Timeout
  {    
    Serial.println("S'ha produit un Timeout del Servidor !!");
    client.stop();
    return;
  }
  }
  //Llegir resposta del Servidor Web (HTTP 200...)
  while (client.available()){ 
  String line=client.readStringUntil('\r');
  Serial.print(line); //Mostra bytes rebuts
  int valor=line.toInt();
  }

  //Enviar una lectura cada X milisegons aprox. (EL REPTE DEMANA CADA 15s però està a 2 per generar dades pel tests amb sql i estadística)
  Serial.println();                
  Serial.println("Tancant connexió");     
  delay(2000);            //Enviar una lectura cada X milisegons aprox. (EL REPTE DEMANA CADA 15s però està a 2 per generar dades pel tests amb sql i estadística)
}
