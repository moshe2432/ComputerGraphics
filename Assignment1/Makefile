# macOS/Linux default.
CXX := g++
CXXFLAGS := -std=c++17 -Wall -O2 -Iexternal
# Windows/MinGW-only static runtime options (commented out):
# CXX := C:/Users/moshe/tools/mingw64/bin/g++.exe
# LDFLAGS := -static-libgcc -static-libstdc++ -static
LDFLAGS :=
SRC := src/main.cpp src/Image.cpp src/stb_impl.cpp
TARGET := assignment1

.PHONY: all run clean

all: $(TARGET)

$(TARGET): $(SRC) src/Image.h
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)

run: all
	./$(TARGET)

clean:
	rm -f $(TARGET) $(TARGET).exe
