CPP = g++
C_FLAGS = -fPIC -g -c -DGL_GLEXT_PROTOTYPES $(INC)

LINK = g++ -fPIC -g



OBJS = Main.o Grid.o

project1: $(OBJS)  
	$(LINK) -o project1 $(OBJS) 


Main.o: Main.c++
	$(CPP) $(C_FLAGS) Main.c++


Grid.o: Grid.h Grid.c++
	$(CPP) $(C_FLAGS) Grid.c++ 
	
clean:
	rm *.o


