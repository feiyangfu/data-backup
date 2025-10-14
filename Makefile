CXX = g++

CXXFLAGS = -std=c++17 -Wall -g

SRCDIR = src

TARGET = data_backup

SOURCES = $(wildcard $(SRCDIR)/*.cpp)

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SOURCES)

clean:
	rm -f $(TARGET)

.PHONY: all clean