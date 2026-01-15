.PHONY: default clean

CFLAGS = -Og -g -I. -Ilib/winblit -DCLIPPING_METHOD=SCREEN_SPACE_CLIPPING -DRASTERIZATION_METHOD=SCANLINE_RASTERIZATION
LDFLAGS = -Og -g -L. -lm -mwindows -mconsole -lwinmm -Llib/winblit -lwinblit

TARGET = a.exe

default: $(TARGET)
	./$(TARGET)

$(TARGET): main.o draw.o rml/rml.o clock.o stb.o clip.o
	$(MAKE) -C lib
	$(CC) main.o draw.o rml/rml.o clock.o stb.o clip.o $(LDFLAGS) -o$(TARGET)

main.o: main.c
	$(CC) -c main.c $(CFLAGS)
draw.o: draw.c
	$(CC) -c draw.c $(CFLAGS)
rml/rml.o: rml/rml.c
	$(CC) -c rml/rml.c $(CFLAGS) -orml/rml.o
clock.o: clock.c
	$(CC) -c clock.c $(CFLAGS)
stb.o: stb.c
	$(CC) -c stb.c $(CFLAGS)
clip.o: clip.c
	$(CC) -c clip.c $(CFLAGS)

clean:
	$(RM) *.o *.exe
