SRC=$(wildcard ./*.cpp)
OBJ=$(patsubst %.cpp, %.o, $(SRC))
BIN=./server

$(BIN):$(OBJ)
	g++ $^ -o $@  -L../../common/lib -lpthread -lconfig -lparseCfg -lmysqlclient
./%.o:./%.cpp
	g++ -c $< -o $@ -g 

.PHONY:clean
clean:
	rm -rf $(BIN) $(OBJ)
