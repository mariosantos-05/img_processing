CC = "g++"
PROJECT = Trabalho_01
SRC = main.cpp


LIBS = `pkg-config opencv4 --cflags --libs`

$(PROJECT) : $(SRC)
	$(CC) $(SRC) -o $(PROJECT) $(LIBS)