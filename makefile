
BIN = bin/dgen
SRCS = $(wildcard src/*.cpp)
OBJS = $(SRCS:src/%.cpp=out/%.o)
DEPS = $(SRCS:src/%.cpp=out/%.d)

CFLAGS = -c -MMD -I inc -Wall -O2
LDFLAGS = -lsfml-graphics -lsfml-window -lsfml-system

all: $(BIN)

-include $(DEPS)

out:
	mkdir out

out/%.o: src/%.cpp | out
	$(CXX) $(CFLAGS) $< -o $@

bin:
	mkdir bin

$(BIN): $(OBJS) | bin
	$(CXX) $^ $(LDFLAGS) -o $@

clean:
	rm -rf out bin

test: all
	$(BIN)