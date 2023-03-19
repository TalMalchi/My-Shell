#do make file for myshell2.cpp


shell2: shell2.o
	g++ -o shell2 shell2.o

shell2.o: shell2.cpp
	g++ -c shell2.cpp

clean:
	rm -f shell2 file *.o