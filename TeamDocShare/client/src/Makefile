SRC=$(wildcard ./*.cpp)
OBJ=$(patsubst %.cpp, %.o, $(SRC))
BIN=./client

$(BIN):$(OBJ)
	g++ $^ -o $@  -L../lib -lpthread -lconfig
./%.o:./%.cpp
	g++ -c $< -o $@ -g 

.PHONY:clean
clean:
	rm -rf $(BIN) $(OBJ)
