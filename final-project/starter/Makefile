CXX = g++
CXXFLAGS = -std=c++11 -Wall

SRCS = main.cpp Stop.cpp BusStop.cpp TrainStop.cpp Route.cpp
OBJS = $(SRCS:.cpp=.o)
TARGET = campusnav

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $<

clean:
	rm -f $(OBJS) $(TARGET)
