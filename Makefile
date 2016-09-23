main:	main.o globalManager.o
	g++ main.o globalManager.o -o main -lpthread

main.o:
	g++ main.cpp -c

globalManager.o:
	g++ globalManager.cpp -c

clean:
	rm -f main *.o
