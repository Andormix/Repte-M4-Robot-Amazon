# importacions
# import mysql.connector
import socket
import os
from datetime import datetime
from flask import Flask, render_template, request #Faciliten tasques de servidor
from gpiozero import LED
from time import sleep

import mysql.connector
from mysql.connector import Error


#Obrim el servidor mysql
try:
    connection = mysql.connector.connect(host='192.168.3.10',
                                         database='juanreptem3',
                                         user='juan',
                                         password='juan')
    if connection.is_connected():
        db_Info = connection.get_server_info()
        print("Connected to MySQL Server version ", db_Info)
        cursor = connection.cursor()
        cursor.execute("select database();")
        record = cursor.fetchone()
        print("You're connected to database: ", record)

except Error as e:
    print("Error while connecting to MySQL", e)

# Instancia de Flask.
app = Flask(__name__)

def adrIP():
	gw = os.popen("ip -4 route show default").read().split()
	s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
	s.connect((gw[2], 0))
	ipaddr = s.getsockname()[0]
	gateway = gw[2]
	host = socket.gethostname()
	print ("Adreça IP:", ipaddr, " Gateway:", gateway, " Nom del Host:", host)
	return ipaddr
# Rutes, cas d'invocar l'arrel del servidor
@app.route('/')
def inici():
	return ("<h2>Rebuda una petició, Servidor web actiu</h2>")

@app.route('/led',methods=['GET'])
def intermitent():
	valor=""
	valor=request.args.get('led')
	print("Led : " + valor)
	if valor=="on":
		led_roja.on()
	if valor=="off":
		led_roja.off()
	request.close()
	return "Led : " + valor

# Rebre dades via GET Exemple des de Navegador:
# http://192.168.13.148:8080/dades?Temperatura=22&Humetat=22 
@app.route('/dades',methods=['GET'])
def dades():

	# Agafem les dades
	temp = request.args.get('Temperatura')
	hum = request.args.get('Humetat')
	co2 = request.args.get('CO2')
	tvoc = request.args.get('tvoc')
	Switch1 = request.args.get('Switch1')
	Switch2 = request.args.get('Switch2')
	#Switch1 = True # Puesto en true para test
	#Switch2 = True # Puesto en true para test
	current = datetime.now()

	codiRetorn=""  #Inicialment 
	canvi1=float(hum) #Casting
	canvi2=float(temp)
	canvi3=float(co2)
	canvi4=float(tvoc)

	print("Humetat :", canvi1, "Temperatura :", canvi2, "CO2: ", canvi3, "TVOC: ", canvi4) # Veure valors en consola
	
    # Enviem dades a SQL per la part d'estadistica sin acento
	mycursor = connection.cursor()
	query = "INSERT INTO Dades (DadesID, Humetat, Temperatura, CO2, TVOC) VALUES (%s, %s, %s, %s, %s)"
	Data1 = (int(current.strftime("%H%M%S")), hum, temp, co2, tvoc)
	mycursor.execute(query, Data1)
	connection.commit()
	
	if canvi1 > 25: # Si humetat >25 codiRetorn 23 : Encendre led
		#Encendre Led Alarma i activar Ventilador.....
		led_roja.on() # Led vermella
		codiRetorn="<p>Massa Humetat</p>"
	if canvi2 > 20: # Si temp >25 codiRetorn 23 : Encendre led
		#Encendre Led Alarma i tancar Calefacció.....
		codiRetorn+="<p>Temperatura alta</p>"
		led_roja.on()
	if codiRetorn=="":
		led_roja.off()
		codiRetorn="Tot en ordre cap problema"
	if Switch1 == "1":
		print(" Hey! Obstacle a la vista!")
		led_aviso_obstaculo.on() # Activaremos alarma con los reles
	else:
		led_aviso_obstaculo.off()
	if Switch2 == "1":
		print(" Hey! Hem arribat al final!")
		#led_aviso_final.on() # Activariamos alarma con los reles
	return codiRetorn 


if __name__ == '__main__':
	led_roja = LED(17) # Led roja aviso mucha humedad o temp
	led_aviso_obstaculo = LED(11) 
	led_aviso_final = LED(26)
	host=adrIP()
	port=8080
	host="192.168.13.238"
	app.run(host=host,port=port)
	
	