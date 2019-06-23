CXX=g++
CXXFLAGS+=-std=c++14 -Wall -Wextra -pedantic
APP=model
DOC=documentation
OBJ=main.o model.o database.o libs/pugixml-1.9/pugixml.o
LLIBS=-lsimlib -lm

$(APP): $(OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LLIBS)

all: $(APP)

run:
	./$(APP)

clean:
	rm *.o
