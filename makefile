CXXFLAGS = -std=c++14

TARGET = run_tests

all: $(TARGET)

$(TARGET): ./test_tool/main.cpp
	$(CXX) $(CXXFLAGS) -I"./src" -I"./submodules/Catch/include" -o $(TARGET) ./test_tool/main.cpp ./test_tool/testcases.cpp ./test_tool/test_*.cpp

clean:
	rm ./test_tool/$(TARGET)

test:
	./$(TARGET)