INC=./includes
SRC=./src
OUT=./bin


$(OUT)/lab: $(OUT)/main.o $(OUT)/common.o $(OUT)/child.o $(OUT)/std.o
	gcc -o $(OUT)/lab $(OUT)/main.o $(OUT)/common.o $(OUT)/child.o $(OUT)/std.o

$(OUT)/main.o: $(SRC)/main.c $(INC)/common.h
	gcc -I $(INC) -o $(OUT)/main.o -c $(SRC)/main.c

$(OUT)/common.o: $(SRC)/common.c $(INC)/common.h $(INC)/child.h $(INC)/std.h
	gcc -I $(INC) -o $(OUT)/common.o -c $(SRC)/common.c

$(OUT)/child.o: $(SRC)/child.c $(INC)/child.h
	gcc -I $(INC) -o $(OUT)/child.o -c $(SRC)/child.c

$(OUT)/std.o: $(SRC)/std.c $(INC)/std.h
	gcc -I $(INC) -o $(OUT)/std.o -c $(SRC)/std.c

clear: clean
clean:
	rm -f $(OUT)/*.o $(OUT)/lab
