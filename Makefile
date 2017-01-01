all: src/clinterp.o
	ar rcs lib/libclinterp.a src/clinterp.o
	cp src/clinterp.hpp inc/clinterp.hpp

src/clinterp.o: src/clinterp.cpp
	g++ -c -std=c++11 -o src/clinterp.o src/clinterp.cpp
clean:
	rm -f bin/*
	rm -f src/*.o
	rm -f lib/*.a
	rm -f inc/*.hpp
