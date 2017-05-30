CXX = g++
CXXFLAGS = -std=c++14

TARGET = run_tests

all: $(TARGET)

$(TARGET): main.cpp
	$(CXX) $(CXXFLAGS) -I"./src" -I"./submodules/Catch/include" -o $(TARGET) ./test_tool/main.cpp ./test_tool7testcases.cpp ./test_tool/test_*.cpp

clean:
	rm $(TARGET)

test:
	./run_tests