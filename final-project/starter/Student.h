#ifndef STUDENT_H
#define STUDENT_H

#include "Person.h"

class Student : public Person {
private:
  double gpa;

public:
  Student(std::string n, int i, double g);
  void display() const override;
  std::string getRole() const override;
};

#endif
