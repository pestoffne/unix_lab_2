INC=./includes
SRC=./src
OUT=./bin


$(OUT)/lab: $(OUT)/main.o $(OUT)/hello.o
	gcc -o $(OUT)/lab $(OUT)/main.o $(OUT)/hello.o 

$(OUT)/main.o: $(SRC)/main.c $(INC)/hello.h
	gcc -I$(INC) -o $(OUT)/main.o -c $(SRC)/main.c

$(OUT)/hello.o: $(SRC)/hello.c $(INC)/hello.h
	gcc -I$(INC) -o $(OUT)/hello.o -c $(SRC)/hello.c

clean:
	rm $(OUT)/*.o $(OUT)/lab
