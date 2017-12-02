CXX = g++
CXXFLAGS = -std=c++14

TARGET = run_tests.exe

all: $(TARGET)

$(TARGET): main.cpp
	$(CXX) $(CXXFLAGS) -I"../src" -I"../submodules/Catch/single_include" -o $(TARGET) main.cpp testcases.cpp test_*.cpp

clean:
	del $(TARGET)