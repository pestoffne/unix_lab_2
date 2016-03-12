INC=./includes
SRC=./src
OUT=./bin


$(OUT)/lab: $(OUT)/main.o $(OUT)/common.o $(OUT)/script.o
	gcc -o $(OUT)/lab $(OUT)/main.o $(OUT)/common.o $(OUT)/script.o

$(OUT)/main.o: $(SRC)/main.c $(INC)/common.h
	gcc -I $(INC) -o $(OUT)/main.o -c $(SRC)/main.c

$(OUT)/common.o: $(SRC)/common.c $(INC)/common.h $(INC)/script.h
	gcc -I $(INC) -o $(OUT)/common.o -c $(SRC)/common.c

$(OUT)/script.o: $(SRC)/script.c $(INC)/script.h
	gcc -I $(INC) -o $(OUT)/script.o -c $(SRC)/script.c

clean:
	rm $(OUT)/*.o $(OUT)/lab
