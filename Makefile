VERSION = 1.0.0

rtsim.out : laplace.h laplace.a main.cpp
	g++ -Wall -Llaplace main.cpp -o rtsim.out

laplace.a : laplace.a.${VERSION}
	ln -s laplace.a.${VERSION} laplace.a

laplace.a.${VERSION} : laplace.o
	g++ -static -o laplace.a.${VERSION} laplace.o

laplace.o : laplace.cpp laplace.h
	g++ -Wall -g -c laplace.cpp -o laplace.o

clean:
	rm -rf *.o *.a* *.out





