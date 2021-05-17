VERSION = 1.0.0

rtsim.out : laplace.h laplace.so main.cpp Zeit.h
	g++ -Wall -Llaplace main.cpp -o rtsim.out

laplace.so : laplace.so.${VERSION}
	ln -s laplace.so.${VERSION} laplace.so

laplace.so.${VERSION} : laplace.o
	g++ -shared -fPIC -o laplace.so.${VERSION} laplace.o

laplace.o : laplace.cpp laplace.h Zeit.h
	g++ -Wall -g -c laplace.cpp -o laplace.o

clean:
	rm -rf *.o *.so* rtsim





