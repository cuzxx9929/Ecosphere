main:main.o globalManager.o
	g++ globalManager.o main.o -o main
main.o:
	g++ main.cpp -c
globalManager.o:
	g++ globalManager.cpp -c
clean:
	rm -f main *.o
