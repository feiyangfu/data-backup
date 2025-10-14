# 编译器
CXX = g++

# 编译选项
CXXFLAGS = -std=c++17 -Wall -g

# GTest 相关的路径和编译选项 (已应用修复1)
GTEST_DIR = lib/googletest
CPPFLAGS = -I$(GTEST_DIR)/googletest -I$(GTEST_DIR)/googletest/include -I$(GTEST_DIR)/googlemock/include -pthread

# 源文件目录
SRCDIR = src

# 主程序
TARGET = data_backup
MAIN_SRC = $(SRCDIR)/main.cpp

# 核心逻辑
CORE_SRC = $(SRCDIR)/backup.cpp
CORE_OBJ = $(CORE_SRC:.cpp=.o)

# 测试程序
TEST_TARGET = run_tests
TEST_SRC = test/test_main.cpp
GTEST_SRCS = $(GTEST_DIR)/googletest/src/gtest-all.cc $(GTEST_DIR)/googletest/src/gtest_main.cc

# 默认目标
all: $(TARGET) $(TEST_TARGET)

# --- 编译核心逻辑 ---
$(CORE_OBJ): $(CORE_SRC)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

# --- 编译和链接主程序 ---
$(TARGET): $(MAIN_SRC) $(CORE_OBJ)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(MAIN_SRC) $(CORE_OBJ)

# --- 编译和链接测试程序 ---
$(TEST_TARGET): $(GTEST_SRCS) $(TEST_SRC) $(CORE_OBJ)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o $(TEST_TARGET) $(GTEST_SRCS) $(TEST_SRC) $(CORE_OBJ)

# --- 运行和清理 ---
test: $(TEST_TARGET)
	./$(TEST_TARGET)

clean:
	rm -f $(TARGET) $(TEST_TARGET) $(SRCDIR)/*.o *.o test_source.txt test_dest.txt

.PHONY: all clean test