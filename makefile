CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -pedantic

# 源文件目录
SRC_DIR = ./http
# 输出目录
BUILD_DIR = ./build

# 源文件列表
SRCS = $(wildcard $(SRC_DIR)/*.cpp) ./test_http.cpp
# 目标文件列表（替换源文件目录为输出目录）
OBJS = $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(filter-out ./test_http.cpp, $(SRCS))) $(BUILD_DIR)/test_http.o

# 目标可执行文件
TARGET = http_server

# 默认目标
all: $(TARGET)

# 编译目标
$(TARGET): $(OBJS)
    $(CXX) $(CXXFLAGS) $^ -o $@

# 编译每个源文件为目标文件
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
    @mkdir -p $(BUILD_DIR)
    $(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/test_http.o: ./test_http.cpp
    @mkdir -p $(BUILD_DIR)
    $(CXX) $(CXXFLAGS) -c $< -o $@

# 清理
clean:
    @rm -rf $(BUILD_DIR)
    @rm -f $(TARGET)

# 声明伪目标
.PHONY: all clean
