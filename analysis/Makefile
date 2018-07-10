CC=g++
CXXFLAGS=`root-config --cflags --libs` -shared -fPIC
SRCS=

all: $(SRCS)

%.so: %.cxx %.h
	$(CC) $(CXXFLAGS) $< -o $@

clean:
	rm *.so
