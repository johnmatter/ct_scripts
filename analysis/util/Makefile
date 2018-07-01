CC=g++
CXXFLAGS=`root-config --cflags --libs` -shared -fPIC
LIBS=CTChains.so CTData.so

all: $(LIBS)

%.so: %.cxx %.h
	$(CC) $(CXXFLAGS) $< -o $@

clean:
	rm *.so
