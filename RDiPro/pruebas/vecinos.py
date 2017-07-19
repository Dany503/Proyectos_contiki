f = open("txt/tabla-vecinos.txt",'r') # Comentario

number = raw_input("Introduce el ID del nodo (0 para seleccionar todos): ")
tabla = []

for linea in f:
	#print linea
	campos = linea.split("	")
	ident = campos[1].split(":")[1]
	#print ident
	
	if number == '0':
		tabla.append([])
		mensaje = campos[2].split(" ")
		if mensaje[0] == "[tabla":
			tabla[int(ident)].append(int(mensaje[3].split(".")[0]))
	else:
		if ident == number:
			mensaje = campos[2].split(" ")
			if mensaje[0] == "[tabla":
				#print mensaje[3]
				tabla.append(int(mensaje[3].split(".")[0]))

if number == '0':
	nodos = int(raw_input("Introduce el numero de nodos: "))
	# El valor de j funciona realmente, saca el numero de elementos en tabla, el problema es que hay 200 elementos
	#j = sum(len(x) for x in tabla)
	#print tabla
	for i in range(1,nodos+1):
		tabla[i] = set(tabla[i])
		print "Los vecinos de",i,"son:	"," - ".join(str(x) for x in tabla[i])
else:
	tabla = set(tabla)
	print "Los vecinos de",number,"son: "," - ".join(str(x) for x in tabla)
