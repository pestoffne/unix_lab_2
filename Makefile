INC=./includes
SRC=./src
OUT=./bin


manager: $(OUT)/main.o $(OUT)/async.o $(OUT)/select.o $(OUT)/common.o
	gcc -o manager $(OUT)/main.o $(OUT)/async.o $(OUT)/select.o $(OUT)/common.o

$(OUT)/main.o: $(SRC)/main.c $(INC)/async.h $(INC)/select.h
	gcc -I $(INC) -o $(OUT)/main.o -c $(SRC)/main.c

$(OUT)/async.o: $(SRC)/async.c $(INC)/async.h $(INC)/common.h
	gcc -I $(INC) -o $(OUT)/async.o -c $(SRC)/async.c

$(OUT)/select.o: $(SRC)/select.c $(INC)/select.h $(INC)/common.h
	gcc -I $(INC) -o $(OUT)/select.o -c $(SRC)/select.c

$(OUT)/common.o: $(SRC)/common.c $(INC)/common.h
	gcc -I $(INC) -o $(OUT)/common.o -c $(SRC)/common.c

clear: clean
clean:
	rm -f $(OUT)/*.o manager
