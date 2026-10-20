# Final Project: Roster Manager

**Points:** TBD. **Due:** TBD (Pacific time).

## What you're building

A console C++ program that manages a roster of people (students and
instructors) using the ideas from Modules 5–10: a base class, two
derived classes, polymorphism through a base-class pointer, a vector
of records, and saving/loading data with file streams.

The starter code in `starter/` already compiles and runs — it's your
foundation, not a blank page. Extend it; don't start over.

## Starter code structure

| File | What it defines |
|---|---|
| `Person.h` / `Person.cpp` | Base class: name, id, virtual `display()`/`getRole()` |
| `Student.h` / `Student.cpp` | Derived class: adds a GPA, overrides `display()`/`getRole()` |
| `Instructor.h` / `Instructor.cpp` | Derived class: adds a department, overrides `display()`/`getRole()` |
| `Roster.h` / `Roster.cpp` | Holds a `vector<Person*>`, can add/display/save records |
| `main.cpp` | Builds a small roster and demonstrates each piece |
| `Makefile` | Builds everything with one `make` command |

A single-file version (everything combined into one `.cpp`, for pasting
into an online compiler) is in `single-file/roster_manager.cpp`.

## Minimum requirements

- At least one more derived class of your own (e.g. `TA`, `Staff`) that
  overrides `display()`/`getRole()`.
- At least one new field/behavior on the existing classes that's
  meaningfully yours, not just a renamed copy of what's here.
- `Roster::saveToFile()` and a matching `loadFromFile()` that can read
  the data back in.
- A short `README` section (add it to this file, or a new one) telling
  a reader what your program does and how to run it.

## Submission

Submit via Canvas. Groups OK, but by submitting you're confirming you
personally understand the work. AI is fine to use, but clearly state
which parts used AI and which parts you did without it.
