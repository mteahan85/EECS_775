project1: Main.o Grid.o
	g++ Main.o Grid.o -o project1

main.o: Main.cpp Grid.h
	g++ Main.cpp -c -g

grid.o: Grid.h Grid.cpp
	g++ Grid.cpp -c -g
	
clean:
	rm *.o


