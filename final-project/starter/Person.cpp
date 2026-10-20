#include "Person.h"
#include <iostream>
using namespace std;

Person::Person(string n, int i) : name(n), id(i) {}

void Person::display() const {
  cout << "[" << getRole() << "] " << name
       << " (ID " << id << ")" << endl;
}

string Person::getRole() const {
  return "Person";
}
