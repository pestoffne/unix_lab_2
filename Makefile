INC=./includes
SRC=./src
OUT=./bin


manager: $(OUT)/main.o
	gcc -o manager $(OUT)/main.o

$(OUT)/main.o: $(SRC)/main.c
	gcc -I $(INC) -o $(OUT)/main.o -c $(SRC)/main.c

clear: clean
clean:
	rm -f $(OUT)/*.o $(OUT)/lab
