Los archivos estan organizados en dos carpetas principales

* contiki : Donde se incluye el código desarrollado
	* core : los archivos de librería que necesita el código principal. Incluir el contenido en la carpeta ya existente, si existe, sobreescribir estos.
	* codigos : Los algoritmos desarrollados, tanto nedflood como rdipro (pueden ir en cualquier carpeta, el make es genérico con ruta absoluta).
* pruebas : se almacenan los script python y los resultados de las simulaciones.
	* / : script python que recogen los datos de los txt
	* csc : archivos de simulación Cooja
	* graficas_python : script python que generan las imágenes de las gráficas (graficas incluidas)
	* txt : registro de las simulaciones con formato [algoritmo]{N}n{M}m.txt donde N es el numero de nodos y M el número de mensajes.
