INC=./includes
SRC=./src
OUT=./bin


manager: $(OUT)/main.o $(OUT)/process.o
	gcc -o manager $(OUT)/main.o $(OUT)/process.o

$(OUT)/main.o: $(SRC)/main.c $(INC)/process.h
	gcc -I $(INC) -o $(OUT)/main.o -c $(SRC)/main.c

$(OUT)/process.o: $(SRC)/process.c $(INC)/process.h
	gcc -I $(INC) -o $(OUT)/process.o -c $(SRC)/process.c

clear: clean
clean:
	rm -f $(OUT)/*.o $(OUT)/lab
