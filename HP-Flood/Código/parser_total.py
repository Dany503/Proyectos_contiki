from decimal import getcontext, Decimal
import matplotlib.pyplot as plt; plt.rcdefaults()
import numpy as np
import matplotlib.pyplot as plt

getcontext().prec = 3 #definimos la precision de los decimales

f25 = open("res25.txt",'r')
f50 = open("res50.txt",'r')
f100 = open("res100.txt",'r')
f125 = open("res125.txt",'r')
f150 = open("res150.txt",'r')

f25_hp = open("res25_hp.txt",'r')
f50_hp = open("res50_hp.txt",'r')
f100_hp = open("res100_hp.txt",'r')
f125_hp = open("res125_hp.txt",'r')
f150_hp = open("res150_hp.txt",'r')

recibidores = [0,0,0,0,0]
recibidores_hp = [0,0,0,0,0]

retransmisiones = [0,0,0,0,0]
retransmisiones_hp = [0,0,0,0,0]

reachability = [0,0,0,0,0]
reachability_hp = [0,0,0,0,0]

n = [25,50,100,125,150]

print
print "##########"
print "##########"
print "Flooding"
print "##########"
print "##########"
print

for linea in f25:
	# calcular reachability: n nodos reciben el mensaje/nodos totales
	campos = linea.split(" ") # devuelve sent, droped o received
	if campos[4] == 'received':
		recibidores[0] = recibidores[0] + 1
	if campos[4] == 'sent':
		retransmisiones[0] = retransmisiones[0] + 1

print "Numero de nodos que han recibido el mensaje para 25 nodos:", recibidores[0]+1 #contando con el emisor
reachability[0] = Decimal(recibidores[0]+1)/Decimal(25)

for linea in f50:
	# calcular reachability: n nodos reciben el mensaje/nodos totales
	campos = linea.split(" ") # devuelve sent, droped o received
	if campos[4] == 'received':
		recibidores[1] = recibidores[1] + 1
	if campos[4] == 'sent':
		retransmisiones[1] = retransmisiones[1] + 1

print "Numero de nodos que han recibido el mensaje para 50 nodos:", recibidores[1]+1 #contando con el emisor
reachability[1] = Decimal(recibidores[1]+1)/Decimal(50)

for linea in f100:
	# calcular reachability: n nodos reciben el mensaje/nodos totales
	campos = linea.split(" ") # devuelve sent, droped o received
	if campos[4] == 'received':
		recibidores[2] = recibidores[2] + 1
	if campos[4] == 'sent':
		retransmisiones[2] = retransmisiones[2] + 1

print "Numero de nodos que han recibido el mensaje para 100 nodos:", recibidores[2]+1 #contando con el emisor
reachability[2] = Decimal(recibidores[2]+1)/Decimal(100)

for linea in f125:
	# calcular reachability: n nodos reciben el mensaje/nodos totales
	campos = linea.split(" ") # devuelve sent, droped o received
	if campos[4] == 'received':
		recibidores[3] = recibidores[3] + 1
	if campos[4] == 'sent':
		retransmisiones[3] = retransmisiones[3] + 1

print "Numero de nodos que han recibido el mensaje para 125 nodos:", recibidores[3]+1 #contando con el emisor
reachability[3] = Decimal(recibidores[3]+1)/Decimal(125)

for linea in f150:
	# calcular reachability: n nodos reciben el mensaje/nodos totales
	campos = linea.split(" ") # devuelve sent, droped o received
	if campos[4] == 'received':
		recibidores[4] = recibidores[4] + 1
	if campos[4] == 'sent':
		retransmisiones[4] = retransmisiones[4] + 1

print "Numero de nodos que han recibido el mensaje para 150 nodos:", recibidores[4]+1 #contando con el emisor
reachability[4] = Decimal(recibidores[4]+1)/Decimal(150)

print
print
print "Reachabilitys obtenidas: "
print

for i in range(5):
	print "Reachability para", n[i], "nodos:", reachability[i]


print
print
print "Retransmisiones: "
print

for i in range(5):
	print "Retransmisiones para", n[i], "nodos:", retransmisiones[i]



print
print "##########"
print "##########"
print "HP-Flood"
print "##########"
print "##########"
print

for linea in f25_hp:
	# calcular reachability: n nodos reciben el mensaje/nodos totales
	campos = linea.split(" ") # devuelve sent, droped o received
	if campos[4] == 'received':
		recibidores_hp[0] = recibidores_hp[0] + 1
	if campos[4] == 'sent':
		retransmisiones_hp[0] = retransmisiones_hp[0] + 1

print "Numero de nodos que han recibido el mensaje para 25 nodos:", recibidores_hp[0]+1 #contando con el emisor
reachability_hp[0] = Decimal(recibidores_hp[0]+1)/Decimal(25)

for linea in f50_hp:
	# calcular reachability: n nodos reciben el mensaje/nodos totales
	campos = linea.split(" ") # devuelve sent, droped o received
	if campos[4] == 'received':
		recibidores_hp[1] = recibidores_hp[1] + 1
	if campos[4] == 'sent':
		retransmisiones_hp[1] = retransmisiones_hp[1] + 1

print "Numero de nodos que han recibido el mensaje para 50 nodos:", recibidores_hp[1]+1 #contando con el emisor
reachability_hp[1] = Decimal(recibidores_hp[1]+1)/Decimal(50)

for linea in f100_hp:
	# calcular reachability: n nodos reciben el mensaje/nodos totales
	campos = linea.split(" ") # devuelve sent, droped o received
	if campos[4] == 'received':
		recibidores_hp[2] = recibidores_hp[2] + 1
	if campos[4] == 'sent':
		retransmisiones_hp[2] = retransmisiones_hp[2] + 1

print "Numero de nodos que han recibido el mensaje para 100 nodos:", recibidores_hp[2]+1 #contando con el emisor
reachability_hp[2] = Decimal(recibidores_hp[2]+1)/Decimal(100)

for linea in f125_hp:
	# calcular reachability: n nodos reciben el mensaje/nodos totales
	campos = linea.split(" ") # devuelve sent, droped o received
	if campos[4] == 'received':
		recibidores_hp[3] = recibidores_hp[3] + 1
	if campos[4] == 'sent':
		retransmisiones_hp[3] = retransmisiones_hp[3] + 1

print "Numero de nodos que han recibido el mensaje para 125 nodos:", recibidores_hp[3]+1 #contando con el emisor
reachability_hp[3] = Decimal(recibidores_hp[3]+1)/Decimal(125)

for linea in f150_hp:
	# calcular reachability: n nodos reciben el mensaje/nodos totales
	campos = linea.split(" ") # devuelve sent, droped o received
	if campos[4] == 'received':
		recibidores_hp[4] = recibidores_hp[4] + 1
	if campos[4] == 'sent':
		retransmisiones_hp[4] = retransmisiones_hp[4] + 1

print "Numero de nodos que han recibido el mensaje para 150 nodos:", recibidores_hp[4]+1 #contando con el emisor
reachability_hp[4] = Decimal(recibidores_hp[4]+1)/Decimal(150)

print
print
print "Reachabilitys obtenidas: "
print

for i in range(5):
	print "Reachability para", n[i], "nodos:", reachability_hp[i]


print
print
print "Retransmisiones: "
print

for i in range(5):
	print "Retransmisiones para", n[i], "nodos:", retransmisiones_hp[i]



#############
#### Representaciones graficas
#############
fig, ax = plt.subplots()
index = np.arange(5)
bar_width = 0.35
opacity = 0.4

y1 = (reachability[0], reachability[1], reachability[2], reachability[3], reachability[4])
y2 = (reachability_hp[0], reachability_hp[1], reachability_hp[2], reachability_hp[3], reachability_hp[4])

rects1 = plt.bar(index, y1, bar_width,
                 alpha=opacity,
                 color='b',
                 label='Flooding')
 
rects2 = plt.bar(index + bar_width, y2, bar_width,
                 alpha=opacity,
                 color='c',
                 label='HP-Flooding')
 
plt.xlabel('Numero de nodos')
plt.ylabel('Reachability')
plt.title('Reachability vs. numero de nodos')
plt.xticks(index + bar_width, ('25', '50', '100', '125', '150'))
plt.legend()
 
plt.tight_layout()
#plt.show()



fig, ax = plt.subplots()
index = np.arange(5)
bar_width = 0.35
opacity = 0.4

y3 = (retransmisiones[0], retransmisiones[1], retransmisiones[2], retransmisiones[3], retransmisiones[4])
y4 = (retransmisiones_hp[0], retransmisiones_hp[1], retransmisiones_hp[2], retransmisiones_hp[3], retransmisiones_hp[4])

rects1 = plt.bar(index, y3, bar_width,
                 alpha=opacity,
                 color='r',
                 label='Flooding')
 
rects2 = plt.bar(index + bar_width, y4, bar_width,
                 alpha=opacity,
                 color='m',
                 label='HP-Flooding')
 
plt.xlabel('Numero de nodos')
plt.ylabel('Numero de retransmisiones')
plt.title('Numero de retransmisiones vs. numero de nodos')
plt.xticks(index + bar_width, ('25', '50', '100', '125', '150'))
plt.legend()
 
plt.tight_layout()
plt.show()
