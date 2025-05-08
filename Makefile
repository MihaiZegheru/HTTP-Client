# Binary
CLIENT_BIN = client

# Logging
LOG_PATH = logs
CLIENT_LOG = $(LOG_PATH)/client.log

# Paths
SRC_PATH = src
INCLUDE_PATH = include
LIB_PATH = lib
EXTERNAL_PATH = external

# Source files (automatically find all .cpp files in src/)
SRCS = $(wildcard $(SRC_PATH)/*.cpp) $(wildcard $(LIB_PATH)/**/src/*.cpp)
CLIENT_SRC = $(SRC_PATH)/client.cpp

# Header files (automatically include all headers in include/ and lib/)
HEADERS = $(wildcard $(INCLUDE_PATH)/*.hpp) $(wildcard $(LIB_PATH)/**/include/**/*.hpp) $(wildcard $(EXTERNAL_PATH)/**/*.hpp)

# C++ flags and compiler
CXXFLAGS = -std=c++17 -Wall -Werror -Wno-error=unused-variable -I$(INCLUDE_PATH) -I./lib/log/include
CXX = g++

# Default target: build everything
all: $(CLIENT_BIN) $(LOG_PATH)

# Debug target: add debug flags
debug: CXXFLAGS += -DDEBUG
debug: clean all

# Build the client binary
$(CLIENT_BIN): $(CLIENT_SRC) $(SRCS)
	$(CXX) $(CXXFLAGS) -DLOG_FILE=\"$(CLIENT_LOG)\" -o $@ $^

# Create log directory
$(LOG_PATH):
	mkdir -p $(LOG_PATH)

# Clean up generated files
clean:
	rm -rf $(CLIENT_BIN) $(LOG_PATH) *.o
