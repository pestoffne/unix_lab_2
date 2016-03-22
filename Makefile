INC=./includes
SRC=./src
OUT=./bin


$(OUT)/lab: $(OUT)/main.o $(OUT)/child.o $(OUT)/std.o $(OUT)/kill.o $(OUT)/pipe.o
	gcc -o $(OUT)/lab $(OUT)/main.o $(OUT)/child.o $(OUT)/std.o $(OUT)/kill.o $(OUT)/pipe.o

$(OUT)/main.o: $(SRC)/main.c $(INC)/child.h $(INC)/std.h $(INC)/kill.h $(INC)/pipe.h
	gcc -I $(INC) -o $(OUT)/main.o -c $(SRC)/main.c

$(OUT)/child.o: $(SRC)/child.c $(INC)/child.h
	gcc -I $(INC) -o $(OUT)/child.o -c $(SRC)/child.c

$(OUT)/std.o: $(SRC)/std.c $(INC)/std.h
	gcc -I $(INC) -o $(OUT)/std.o -c $(SRC)/std.c

$(OUT)/kill.o: $(SRC)/kill.c $(INC)/kill.h
	gcc -I $(INC) -o $(OUT)/kill.o -c $(SRC)/kill.c

$(OUT)/pipe.o: $(SRC)/pipe.c $(INC)/pipe.h
	gcc -I $(INC) -o $(OUT)/pipe.o -c $(SRC)/pipe.c

clear: clean
clean:
	rm -f $(OUT)/*.o $(OUT)/lab
