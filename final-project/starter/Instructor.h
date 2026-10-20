#ifndef INSTRUCTOR_H
#define INSTRUCTOR_H

#include "Person.h"

class Instructor : public Person {
private:
  std::string department;

public:
  Instructor(std::string n, int i, std::string dept);
  void display() const override;
  std::string getRole() const override;
};

#endif
