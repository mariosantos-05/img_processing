CC = "g++"
PROJECT = Trabalho_01
SRC = canny_edge.cpp


LIBS = `pkg-config opencv4 --cflags --libs`

$(PROJECT) : $(SRC)
	$(CC) $(SRC) -o $(PROJECT) $(LIBS)