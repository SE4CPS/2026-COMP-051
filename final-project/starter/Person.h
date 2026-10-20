#ifndef PERSON_H
#define PERSON_H

#include <string>

class Person {
protected:
  std::string name;
  int id;

public:
  Person(std::string n, int i);
  virtual void display() const;
  virtual std::string getRole() const;
  virtual ~Person() {}
};

#endif
