import sys

archivo = str(sys.argv[1])

f = open(archivo,'r')

msg = 0

for linea in f:
	campos = linea.split("	")
	if campos[2].split(" ")[1] != "broadcast_send\n":
		msg+=1

print "El numero total de mensajes de vecinos es: ",msg
