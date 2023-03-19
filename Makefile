#do make file for myshell2.cpp


myshell: shell2.o
	g++ -o myshell shell2.o

shell2.o: shell2.cpp
	g++ -c shell2.cpp

clean:
	rm -f myshell file *.o