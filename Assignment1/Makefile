# Defaults to the portable MinGW-w64 build extracted to C:\Users\moshe\tools\mingw64.
# Override with `make CXX=g++` once a compiler is on PATH.
CXX := C:/Users/moshe/tools/mingw64/bin/g++.exe
CXXFLAGS := -std=c++17 -Wall -O2 -Iexternal
SRC := src/main.cpp src/Image.cpp src/stb_impl.cpp
TARGET := assignment1

.PHONY: all run clean

all: $(TARGET)

$(TARGET): $(SRC) src/Image.h
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)

run: all
	./$(TARGET)

clean:
	rm -f $(TARGET) $(TARGET).exe
