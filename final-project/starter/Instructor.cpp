#include "Instructor.h"
#include <iostream>
using namespace std;

Instructor::Instructor(string n, int i, string dept)
    : Person(n, i), department(dept) {}

string Instructor::getRole() const {
  return "Instructor";
}

void Instructor::display() const {
  Person::display();
  cout << "    Dept: " << department << endl;
}
