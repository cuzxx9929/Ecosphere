main:	main.o worldManager.o
	g++ main.o worldManager.o -o main -lpthread

main.o:
	g++ main.cpp -c

worldManager.o:
	g++ worldManager.cpp -c

clean:
	rm -f main *.o
