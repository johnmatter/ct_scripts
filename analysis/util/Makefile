CC=g++
CXXFLAGS=`root-config --cflags --libs` -shared -fPIC
LIBS=CTData.so

all: $(LIBS)

%.so: %.cxx %.h
	$(CC) $(CXXFLAGS) $< -o $@

clean:
	rm *.so
