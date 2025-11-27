CXX = g++
CXXFLAGS = -std=c++17 -Iinclude -I/opt/homebrew/include -Wall -Wextra -O2
LDFLAGS = -L/opt/homebrew/lib -lxlsxio_read

SRC_DIR = src
OBJ_DIR = obj
BIN = spamfilter

SRCS = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRCS))

all: $(BIN)

$(BIN): $(OBJS)
	$(CXX) $(OBJS) $(LDFLAGS) -o $(BIN)
	@echo "Build complete! Run with ./spamfilter"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(BIN)
	@echo "Clean complete!"

run: all
	./$(BIN)

.PHONY: all clean run
