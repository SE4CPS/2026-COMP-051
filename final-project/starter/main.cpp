#include "Person.h"
#include "Student.h"
#include "Instructor.h"
#include "Roster.h"

int main() {
  Roster roster;

  roster.add(new Student("Maya Chen", 1001, 3.85));
  roster.add(new Instructor("Dr. Berhe", 2001, "Computer Science"));
  roster.add(new Student("Jamal Rivera", 1002, 3.42));

  roster.displayAll();
  roster.saveToFile("roster.txt");

  return 0;
}
