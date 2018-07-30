CC = gcc
CFLAGS  = -g -Wall `sdl2-config --cflags` -I/usr/include/SDL_image.h
LDFLAGS= `sdl2-config --libs` -lSDL2_image

ALL: hide unhide

hide:  hide.o functions.o  
	$(CC) -o hide hide.o functions.o $(CFLAGS) $(LDFLAGS)

unhide:  unhide.o functions.o  
	$(CC) -o unhide unhide.o functions.o $(CFLAGS) $(LDFLAGS)

hide.o:  hide.c functions.h 
	$(CC) -c hide.c $(CFLAGS) $(LDFLAGS) 

unhide.o:  unhide.c functions.h
	$(CC) -c unhide.c $(CFLAGS) $(LDFLAGS)

functions.o:  functions.c functions.h 
	$(CC) -c functions.c $(CFLAGS) $(LDFLAGS)
 
clean: 
	$(RM) hide *.o *~
	$(RM) unhide *.o *~
