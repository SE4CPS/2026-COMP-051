#include "Student.h"
#include <iostream>
using namespace std;

Student::Student(string n, int i, double g)
    : Person(n, i), gpa(g) {}

string Student::getRole() const {
  return "Student";
}

void Student::display() const {
  Person::display();
  cout << "    GPA: " << gpa << endl;
}
