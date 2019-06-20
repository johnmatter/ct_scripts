CC=g++
COMPILEFLAGS=`root-config --cflags --libs` -c -fPIC
LINKFLAGS=`root-config --cflags --libs`
DEPS=Wire.h Plane.h
OBJ=Wire.o Plane.o

all: $(OBJ) dc_eventview

%.o: %.cxx $(DEPS)
	$(CC) -c -o $@ $< $(COMPILEFLAGS)

dc_eventview: $(OBJ) dc_eventview.cxx
	$(CC) -o $@ $^ $(LINKFLAGS)

test: $(OBJ) test.cxx
	$(CC) -o $@ $^ $(LINKFLAGS)

clean:
	rm *.o test
