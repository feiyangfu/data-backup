CXX = g++
CXXFLAGS = -std=c++17 -Wall -g

SRCDIR = src
GTEST_DIR = lib/googletest

# --- 源文件和目标文件 ---
# 核心逻辑
CORE_SRC = $(SRCDIR)/backup.cpp $(SRCDIR)/crypto.cpp
CORE_OBJ = $(CORE_SRC:.cpp=.o)

# 主程序
TARGET = data_backup
MAIN_SRC = $(SRCDIR)/main.cpp
MAIN_OBJ = $(MAIN_SRC:.cpp=.o)

# 测试程序
TEST_TARGET = run_tests
TEST_SRC = test/test_main.cpp
TEST_OBJ = $(TEST_SRC:.cpp=.o)
GTEST_SRCS = $(GTEST_DIR)/googletest/src/gtest-all.cc \
             $(GTEST_DIR)/googletest/src/gtest_main.cc
GTEST_OBJ = $(GTEST_SRCS:.cc=.o)

LDFLAGS = -lssl -lcrypto

# 头文件
HEADERS = $(SRCDIR)/backup.h $(SRCDIR)/archive.h $(SRCDIR)/crypto.h

# --- 编译选项 ---
CPPFLAGS = -I$(SRCDIR) -I$(GTEST_DIR)/googletest/include -I$(GTEST_DIR)/googletest -pthread

# --- 规则 ---
all: $(TARGET) $(TEST_TARGET)

# 链接主程序
$(TARGET): $(MAIN_OBJ) $(CORE_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# 链接测试程序
$(TEST_TARGET): $(TEST_OBJ) $(CORE_OBJ) $(GTEST_OBJ)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o $@ $^ $(LDFLAGS)

# 通用编译规则
%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c -o $@ $<

%.o: %.cc
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c -o $@ $<

# 依赖关系
$(MAIN_OBJ) $(CORE_OBJ) $(TEST_OBJ): $(HEADERS)

test: $(TEST_TARGET)
	./$(TEST_TARGET)

clean:
	rm -f $(TARGET) $(TEST_TARGET) $(SRCDIR)/*.o test/*.o lib/googletest/googletest/src/*.o \
	-rf test_source_dir test_unpack_dir test_archive.dat

.PHONY: all clean test