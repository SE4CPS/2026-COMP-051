#include "Roster.h"
#include <fstream>
#include <iostream>
using namespace std;

Roster::~Roster() {
  for (Person* p : people) {
    delete p;
  }
}

void Roster::add(Person* p) {
  people.push_back(p);
}

void Roster::displayAll() const {
  for (Person* p : people) {
    p->display();
  }
}

void Roster::saveToFile(const string& filename) const {
  ofstream out(filename);
  for (Person* p : people) {
    out << p->getRole() << endl;
  }
  out.close();
  cout << "Saved " << people.size()
       << " records to " << filename << endl;
}
