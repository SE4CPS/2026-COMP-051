CXX = g++
CXXFLAGS = -std=c++17 -Wall
OBJS = Person.o Student.o Instructor.o Roster.o main.o

roster: $(OBJS)
	$(CXX) $(CXXFLAGS) -o roster $(OBJS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $<

clean:
	rm -f *.o roster roster.exe

.PHONY: clean
