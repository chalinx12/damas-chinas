damaschinas: damaschinas.o juego.o
	gcc -g -lm -o damaschinas damaschinas.o juego.o -lGLU -lGL -lglut
 
damaschinas.o : damaschinas.c
	gcc -g -c damaschinas.c

juego.o : juego.c
	gcc -g -c juego.c

clean :
	rm damaschinas *.o
