VERSION = 1.0.0

rtsim.out : laplace.h liblaplace.so main.cpp
	g++ -L$(PWD) -Wall -o rtsim.out main.cpp -llaplace

liblaplace.so : liblaplace.so.${VERSION}
	ln -s -f liblaplace.so.${VERSION} liblaplace.so

liblaplace.so.${VERSION} : laplace.o
	g++ -shared -o liblaplace.so.${VERSION} laplace.o

laplace.o : laplace.cpp laplace.h
	g++ -c -Wall -Werror -fpic laplace.cpp

clean:
	rm -rf *.o *.so* *.out

install:
	cp liblaplace.so /usr/lib/liblaplace.so

purge: clean
	rm -f /usr/lib/liblaplace.so

.PHONY: purge clean install
