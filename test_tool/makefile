CXX = g++
CXXFLAGS = -std=c++14

TARGET = test_cases.exe

all: $(TARGET)

$(TARGET): main.cpp
	$(CXX) $(CXXFLAGS) -I"../src" -I"../submodules/Catch/include" -o $(TARGET) main.cpp testcases.cpp

clean:
	$(RM) $(TARGET)