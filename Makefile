.PHONY: default

CFLAGS =  -I. -Og -g -DCLIPPING_METHOD=SCREEN_SPACE_CLIPPING
LDFLAGS = -lm -mwindows -mconsole -lwinmm -Og -g

default: a.exe
	./a.exe

a.exe: main.o bitmap.o draw.o rml/rml.o clock.o window.o stb.o clip.o
	gcc main.o bitmap.o draw.o rml/rml.o clock.o window.o stb.o clip.o $(LDFLAGS) -oa.exe

main.o: main.c
	gcc -c main.c $(CFLAGS)
bitmap.o: bitmap.c
	gcc -c bitmap.c $(CFLAGS)
draw.o: draw.c
	gcc -c draw.c $(CFLAGS)
rml/rml.o: rml/rml.c
	gcc -c rml/rml.c $(CFLAGS) -orml/rml.o
clock.o: clock.c
	gcc -c clock.c $(CFLAGS)
window.o: window.c
	gcc -c window.c $(CFLAGS)
stb.o: stb.c
	gcc -c stb.c $(CFLAGS)
clip.o: clip.c
	gcc -c clip.c $(CFLAGS)
