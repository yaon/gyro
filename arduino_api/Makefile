#source /opt/clanton-full/X.X.X/environment....

EXE=ledsensor

SRC_DIR=src
OBJ_DIR=obj
BIN_DIR=bin

SRC=$(wildcard src/*.cpp)
SRCCC=$(wildcard src/*.c)

OBJ=$(wildcard obj/*.o)

LDFLAGS = -lpthread

all:
	make clean
	make sourcec
	make sourcecpp
	make $(EXE)

sourcec:
	$(CC) -c $(SRCCC)
	rm $(EXE).o
	mv *.o $(OBJ_DIR)

sourcecpp:
	$(CXX) -c $(SRC)
	mv *.o $(OBJ_DIR)

ultraonly:
	rm bin/*

	make $(EXE)

$(EXE):
	$(CXX) $(SRC_DIR)/$(EXE).c -o $(BIN_DIR)/$(EXE) $(OBJ) $(LDFLAGS)

clean:
	@echo "Cleaning the project...."
	rm -rf bin/* obj/*

#
