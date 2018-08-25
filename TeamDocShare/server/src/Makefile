SRC=$(wildcard ./*.cpp)
OBJ=$(patsubst %.cpp, %.o, $(SRC))
BIN=./server

$(BIN):$(OBJ)
	g++ $^ -o $@  -L../../common/lib -lpthread -lparseCfg -lconfig -lmysqlclient 
./%.o:./%.cpp
	g++ -c $< -o $@ -g -Wall

.PHONY:clean
clean:
	rm -rf $(BIN) $(OBJ)
