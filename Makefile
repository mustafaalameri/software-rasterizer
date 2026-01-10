.PHONY: default clean

CFLAGS = -Og -g -I. -DCLIPPING_METHOD=SCREEN_SPACE_CLIPPING -DRASTERIZATION_METHOD=SCANLINE_RASTERIZATION
LDFLAGS = -Og -g -lm -mwindows -mconsole -lwinmm

TARGET = a.exe

default: $(TARGET)
	./$(TARGET)

$(TARGET): main.o bitmap.o draw.o rml/rml.o clock.o window.o stb.o clip.o
	$(CC) main.o bitmap.o draw.o rml/rml.o clock.o window.o stb.o clip.o $(LDFLAGS) -o$(TARGET)

main.o: main.c
	$(CC) -c main.c $(CFLAGS)
bitmap.o: bitmap.c
	$(CC) -c bitmap.c $(CFLAGS)
draw.o: draw.c
	$(CC) -c draw.c $(CFLAGS)
rml/rml.o: rml/rml.c
	$(CC) -c rml/rml.c $(CFLAGS) -orml/rml.o
clock.o: clock.c
	$(CC) -c clock.c $(CFLAGS)
window.o: window.c
	$(CC) -c window.c $(CFLAGS)
stb.o: stb.c
	$(CC) -c stb.c $(CFLAGS)
clip.o: clip.c
	$(CC) -c clip.c $(CFLAGS)

clean:
	$(RM) *.o *.exe
