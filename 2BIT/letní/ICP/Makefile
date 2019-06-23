MAIN=src/BlockEditor
EXEC=blockeditor

SRC=$(wildcard qt/icp18/backend/**/*.cpp $(MAIN).cpp)
LIBS=$(wildcard qt/icp18/backend/libs/**/*.cpp)

OBJ=$(patsubst %.cpp, %.o, $(SRC))
OBJ+=$(patsubst %.cpp, %.o, $(LIBS))


# zjistime nazev systemu a podle toho rozhodneme o verzi GCC
UNAME := $(shell uname)
ifeq ($(UNAME), Darwin)
	CXX=g++-6.2.0
else
	CXX=g++
endif

CXXFLAGS+=-std=c++14 -Wall -Wextra -pedantic


all: $(EXEC)

$(EXEC): ${OBJ}
	#$(CXX) $(CXXFLAGS) $^ -o $@
	cd src && qmake && make

doxygen: Doxyfile
	doxygen Doxyfile

run:

	./src/$(EXEC)

clean:
	rm qt/icp18/backend/**/*.o qt/icp18/backend/libs/**/*.o $(MAIN).o

pack:
	zip -r blockeditor doc  Doxyfile  examples  libs  Makefile  README.txt src