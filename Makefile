CXX=clang++
CXXFLAGS=-g -Wall -std=c++11
LDFLAGS=-lxerces-c -lboost_locale

BIN=parse_messages
OBJ=parse_messages.o MySAX2Handler.o MyInputSource.o MyBinInputStream.o util.o

all: $(BIN)

$(BIN): $(OBJ)
	$(CXX) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -f $(BIN) $(OBJ)

.PHONY: all clean
