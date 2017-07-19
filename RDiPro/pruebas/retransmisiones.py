import sys

archivo = str(sys.argv[1])

#f = open("rdipro10nodos.txt",'r') # Comentario
f = open(archivo,'r')
#f = open("txt/flood25n1m.txt",'r') # Comentario

msg = 0
rtx = 0
recibido = []

for linea in f:
	campos = linea.split("	")
	ident = int(campos[1].split(":")[1])
	if campos[2].split(" ")[0] == "recv":
		recibido.append(ident)
	elif campos[2] == "sent\n":
		rtx+=1
	elif campos[2] != "dropped\n" and campos[2].split(" ")[0] != "Starting" and campos[2].split(" ")[1] != "broadcast_send\n" and campos[2].split(" ")[2] == "sending":
		msg+=1

recibido = set(recibido)
print "Los nodos que han recibido el mensaje son:\n"," - ".join(str(x) for x in recibido)
print "Total: ",len(recibido)," nodos"
print "El numero total de mensajes es: ",msg
print "El numero total de transmisiones es: ",rtx
