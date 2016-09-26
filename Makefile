main:	main.o  tree.o
	g++ main.o tree.o -o main -lpthread

main.o:
	g++ main.cpp -c

tree.o:
	g++ tree.cpp -c

clean:
	rm -f main *.o
