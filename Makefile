CXX=clang++
CXXFLAGS=-g -Wall -std=c++11
LDFLAGS=-lxerces-c

BIN=parse_messages
OBJ=parse_messages.o MySAX2Handler.o MyInputSource.o MyBinInputStream.o

all: $(BIN)

$(BIN): $(OBJ)
	$(CXX) $(LDFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -f $(BIN) $(OBJ)

.PHONY: all clean
