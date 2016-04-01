CPP = g++
INC = -I../cryphutil -I../glslutil -I../mvcutil
C_FLAGS = -fPIC -g -c -DGL_GLEXT_PROTOTYPES $(INC)

LINK = g++ -fPIC -g
LOCAL_UTIL_LIBRARIES = ../lib/libcryph.so ../lib/libglsl.so ../lib/libmvc.so
OGL_LIBRARIES = -lglfw -lGLU -lGL

OBJS = main.o GriddedData.o $(LOCAL_UTIL_LIBRARIES)

main: $(OBJS)
	$(LINK) -o main $(OBJS) $(OGL_LIBRARIES)

../lib/libcryph.so: ../cryphutil/AffPoint.h ../cryphutil/AffPoint.c++
	(cd ../cryphutil; make)

../lib/libglsl.so: ../glslutil/ShaderIF.h ../glslutil/ShaderIF.c++
	(cd ../glslutil; make)

../lib/libmvc.so: ../mvcutil/Controller.c++ ../mvcutil/ModelView.c++
	(cd ../mvcutil; make)

main.o: main.c++
	$(CPP) $(C_FLAGS) main.c++

GriddedData.o: GriddedData.c++
	$(CPP) $(C_FLAGS) GriddedData.c++
