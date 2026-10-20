// Final Project starter -- single-file version for an online compiler.
// Same code as the multi-file version in ../starter/, combined into
// one file so it can be pasted directly into OnlineGDB.
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
using namespace std;

class Person {
protected:
  string name;
  int id;

public:
  Person(string n, int i) : name(n), id(i) {}

  virtual void display() const {
    cout << "[" << getRole() << "] " << name
         << " (ID " << id << ")" << endl;
  }

  virtual string getRole() const {
    return "Person";
  }

  virtual ~Person() {}
};

class Student : public Person {
private:
  double gpa;

public:
  Student(string n, int i, double g)
      : Person(n, i), gpa(g) {}

  string getRole() const override {
    return "Student";
  }

  void display() const override {
    Person::display();
    cout << "    GPA: " << gpa << endl;
  }
};

class Instructor : public Person {
private:
  string department;

public:
  Instructor(string n, int i, string dept)
      : Person(n, i), department(dept) {}

  string getRole() const override {
    return "Instructor";
  }

  void display() const override {
    Person::display();
    cout << "    Dept: " << department << endl;
  }
};

class Roster {
private:
  vector<Person*> people;

public:
  ~Roster() {
    for (Person* p : people) {
      delete p;
    }
  }

  void add(Person* p) {
    people.push_back(p);
  }

  void displayAll() const {
    for (Person* p : people) {
      p->display();
    }
  }

  void saveToFile(const string& filename) const {
    ofstream out(filename);
    for (Person* p : people) {
      out << p->getRole() << endl;
    }
    out.close();
    cout << "Saved " << people.size()
         << " records to " << filename << endl;
  }
};

int main() {
  Roster roster;

  roster.add(new Student("Maya Chen", 1001, 3.85));
  roster.add(new Instructor("Dr. Berhe", 2001, "Computer Science"));
  roster.add(new Student("Jamal Rivera", 1002, 3.42));

  roster.displayAll();
  roster.saveToFile("roster.txt");

  return 0;
}
