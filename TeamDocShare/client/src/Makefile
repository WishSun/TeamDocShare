SRC=$(wildcard ./*.cpp)
OBJ=$(patsubst %.cpp, %.o, $(SRC))
BIN=./client

$(BIN):$(OBJ)
	g++ $^ -o $@  -L../../common/lib -lpthread -lparseCfg -lconfig 
./%.o:./%.cpp
	g++ -c $< -o $@ -g 

.PHONY:clean
clean:
	rm -rf $(BIN) $(OBJ) core
