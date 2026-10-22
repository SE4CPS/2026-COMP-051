# Final Project: CampusNav

A playable campus-shuttle route navigator, built on classes,
inheritance, and polymorphism. The starter code already compiles
and runs: your job is to extend it, not rebuild it from scratch.

## What's already here

- `Stop`: base class for a stop on the route (name, distance)
- `BusStop`, `TrainStop`: two derived stop types, each with its
  own extra field (bus bay, platform number)
- `Route`: holds the ordered list of stops and plays the game:
  validates stop names, walks the route stop by stop, counts
  down to the destination, applies a rush-hour rule, and reports
  total distance and estimated travel time
- `main.cpp`: the playable loop. Prompts for a starting stop, a
  destination, and the current hour, then plays the trip.

## Sample data file

`starter/route.txt` is a sample route in the format
`loadFromFile()` should read: one stop per line,
`TYPE,name,distance,extra` (`TYPE` is `BUS` or `TRAIN`). It's a
different route than the hardcoded default, so once your
`loadFromFile()` works, the printed route should change to this
one instead of the default:

```
BUS,Argonne Hall,0.0,2
TRAIN,Westgate Station,1.2,1
BUS,Market Square,2.4,3
TRAIN,Harborview Junction,3.8,2
BUS,Pinecrest Commons,5.0,1
TRAIN,Summit Terminal,6.3,4
```

## Minimum requirements

- Add your own derived stop type (not Bus or Train) with a
  meaningfully new field of its own
- Implement `Route::loadFromFile()` (currently a stub that
  always returns `false`) so the route can load from a text
  file instead of the hardcoded default
- A short `README.md` of your own describing what you added
- Keep the game playable: it should still compile and run after
  your changes

## Get the code

Single file (no install needed, paste into [OnlineGDB](https://www.onlinegdb.com/online_c%2B%2B_compiler)):
`single-file/campusnav.cpp`

Local build (needs Git + a C++ compiler):

```
git clone https://github.com/SE4CPS/2026-COMP-051.git
cd 2026-COMP-051/final-project/starter
make
./campusnav
```
