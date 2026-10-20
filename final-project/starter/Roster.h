#ifndef ROSTER_H
#define ROSTER_H

#include "Person.h"
#include <vector>
#include <string>

class Roster {
private:
  std::vector<Person*> people;

public:
  ~Roster();
  void add(Person* p);
  void displayAll() const;
  void saveToFile(const std::string& filename) const;
};

#endif
