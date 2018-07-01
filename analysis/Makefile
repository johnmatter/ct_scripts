CC=g++
CXXFLAGS=`root-config --cflags --libs` -shared -fPIC
SRCS=

all: $(LIBS)

%.so: %.cxx %.h
	$(CC) $(CXXFLAGS) $< -o $@

clean:
	rm *.so
