main:	main.o worldManager.o tree.o
	g++ main.o worldManager.o tree.o -o main -lpthread

main.o:
	g++ main.cpp -c

worldManager.o:
	g++ worldManager.cpp -c

tree.o:
	g++ tree.cpp -c

clean:
	rm -f main *.o
