// GridRunner - COMP-051 Semester Project
// Drive the car with the arrow keys from a random start to a random
// destination on a grid that fills the console window. Press A to
// toggle auto-drive: the car keeps moving in the last direction you
// picked, once per interval, without you pressing anything.
// Random "parallel cars" (shown as R) can also roam the grid on their
// own, moving in a random direction every interval, independent of
// whether your own auto-drive is on or off.
// Hit a wall or the edge and you lose.
// Build in Visual Studio (Windows only - uses <conio.h> and <windows.h>).
#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <windows.h>
#include <conio.h>
using namespace std;

class Grid {
public:
    int rows, cols;
    vector<vector<char>> cells;

    Grid(int r, int c) : rows(r), cols(c), cells(r, vector<char>(c, '.')) {}

    bool isWall(int r, int c) const {
        return cells[r][c] == '#';
    }

    bool isInBounds(int r, int c) const {
        return r >= 0 && r < rows && c >= 0 && c < cols;
    }

    void addRandomWalls(int count) {
        for (int i = 0; i < count; i++) {
            int r = rand() % rows;
            int c = rand() % cols;
            cells[r][c] = '#';
        }
    }
};

class Obstacle {
public:
    virtual void onHit() {
        cout << "Hit!";
    }
    virtual ~Obstacle() {}
};

class Wall : public Obstacle {
public:
    void onHit() override {
        cout << "Crashed into a wall!\n";
    }
};

class BadGridSize : public exception {
public:
    const char* what() const noexcept override {
        return "Grid size must be between 5 and 50";
    }
};

class Car {
public:
    int row, col;

    void moveTo(int r, int c) {
        row = r;
        col = c;
    }
};

pair<int, int> randomCell(int rows, int cols) {
    return { rand() % rows, rand() % cols };
}

void printGrid(const Grid& grid, const Car& car, int destRow, int destCol,
               const vector<Car>& randomCars) {
    for (int r = 0; r < grid.rows; r++) {
        for (int c = 0; c < grid.cols; c++) {
            bool isRandomCar = false;
            for (size_t i = 0; i < randomCars.size(); i++) {
                if (randomCars[i].row == r && randomCars[i].col == c) {
                    isRandomCar = true;
                    break;
                }
            }

            char ch;
            if (r == car.row && c == car.col) ch = 'C';
            else if (r == destRow && c == destCol) ch = 'D';
            else if (isRandomCar) ch = 'R';
            else ch = grid.cells[r][c];

            // A monospace character cell is about twice as tall as it
            // is wide, so printing a trailing space after each one
            // doubles its on-screen width -- that's what makes an
            // NxN grid actually look square instead of twice as wide
            // as it is tall.
            cout << ch << ' ';
        }
        cout << "\n";
    }
}

bool checkWin(const Car& car, int destRow, int destCol) {
    return car.row == destRow && car.col == destCol;
}

// Picks one of the 4 directions at random and moves the car there if
// it's a legal move. If the random pick is a wall or off the grid, the
// car just stays put for this tick and tries again next time.
void moveRandomCar(Car& c, const Grid& grid) {
    int dirs[4] = { 72, 80, 75, 77 }; // Up, Down, Left, Right
    int pick = dirs[rand() % 4];

    int newRow = c.row;
    int newCol = c.col;
    switch (pick) {
        case 72: newRow--; break;
        case 80: newRow++; break;
        case 75: newCol--; break;
        case 77: newCol++; break;
    }

    if (grid.isInBounds(newRow, newCol) && !grid.isWall(newRow, newCol)) {
        c.moveTo(newRow, newCol);
    }
}

// Reads the visible console window size in characters. Falls back to a
// sane default (80x25) if the console info isn't available.
bool getConsoleSize(int& cols, int& rows) {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
        cols = csbi.srWindow.Right - csbi.srWindow.Left + 1;
        rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
        return true;
    }
    cols = 80;
    rows = 25;
    return false;
}

string directionName(int key) {
    switch (key) {
        case 72: return "Up";
        case 80: return "Down";
        case 75: return "Left";
        case 77: return "Right";
        default: return "none";
    }
}

int main() {
    srand((unsigned)time(0));

    int consoleCols, consoleRows;
    getConsoleSize(consoleCols, consoleRows);

    // Leave a few lines below the grid for the status line and the
    // win/lose message, and one less column so the last cell doesn't
    // land exactly on the console's auto-wrap edge. Each printed cell
    // is now 2 characters wide (see printGrid), so divide the usable
    // width by 2 to get the actual column count -- this is also what
    // makes a square grid come out visually square on screen.
    int gridCols = (consoleCols - 1) / 2;
    int gridRows = consoleRows - 4;

    try {
        if (gridCols < 5 || gridCols > 50) throw BadGridSize();
        if (gridRows < 5 || gridRows > 50) throw BadGridSize();
    } catch (BadGridSize& e) {
        cout << e.what() << " -- clamping grid to fit.\n";
        if (gridCols < 5) gridCols = 5;
        if (gridCols > 50) gridCols = 50;
        if (gridRows < 5) gridRows = 5;
        if (gridRows > 50) gridRows = 50;
    }

    Grid grid(gridRows, gridCols);
    grid.addRandomWalls(gridRows * gridCols / 10); // ~10% of cells are walls

    Car car;
    pair<int, int> start = randomCell(gridRows, gridCols);
    car.moveTo(start.first, start.second);

    pair<int, int> dest = randomCell(gridRows, gridCols);
    while (dest == start) dest = randomCell(gridRows, gridCols);

    vector<Obstacle*> obstacles;
    Wall wall;
    obstacles.push_back(&wall);

    cout << "Enter auto-drive interval in seconds (e.g. 1.0): ";
    double moveIntervalSec = 1.0;
    cin >> moveIntervalSec;
    if (moveIntervalSec <= 0) moveIntervalSec = 1.0;

    cout << "How many random parallel cars? (0 for none): ";
    int numRandomCars = 0;
    cin >> numRandomCars;
    if (numRandomCars < 0) numRandomCars = 0;

    vector<Car> randomCars(numRandomCars);
    for (int i = 0; i < numRandomCars; i++) {
        pair<int, int> spot;
        do {
            spot = randomCell(gridRows, gridCols);
        } while (grid.isWall(spot.first, spot.second) || spot == start || spot == dest);
        randomCars[i].moveTo(spot.first, spot.second);
    }

    bool gameOver = false;
    bool autoDrive = false;
    int currentDir = 0; // last direction picked; 0 = none yet
    auto lastMoveTime = chrono::steady_clock::now();        // player's own auto-drive tick
    auto lastRandomMoveTime = chrono::steady_clock::now();  // random cars' tick
    auto startTime = chrono::steady_clock::now();           // whole-run timer

    bool needRedraw = true; // only redraw when something actually changed

    while (!gameOver) {
        // The clock itself ticks every loop (not just on needRedraw), so the
        // on-screen timer updates every second even while nothing else changed.
        int elapsedWhole = (int)chrono::duration<double>(
            chrono::steady_clock::now() - startTime).count();
        static int lastShownSec = -1;
        if (elapsedWhole != lastShownSec) {
            lastShownSec = elapsedWhole;
            needRedraw = true;
        }

        if (needRedraw) {
            system("cls");
            printGrid(grid, car, dest.first, dest.second, randomCars);
            cout << "Time: " << elapsedWhole << "s | ";
            cout << "Arrows move. A = toggle auto-drive (" << moveIntervalSec << "s). "
                 << "Auto-drive: " << (autoDrive ? "ON" : "OFF")
                 << ", direction: " << directionName(currentDir);
            if (!autoDrive && currentDir == 0) {
                cout << " (press an arrow key first, then A)";
            }
            cout << "\n";
            needRedraw = false;
        }

        int newRow = car.row;
        int newCol = car.col;
        int key = -1;
        bool playerMoved = false;

        // Always a non-blocking key check (even in manual mode), so the
        // random cars keep moving on their own interval whether or not
        // the player is pressing anything.
        if (_kbhit()) {
            key = _getch();
            if (key == 224) key = _getch();
        }

        if (key == 'a' || key == 'A') {
            if (autoDrive) {
                autoDrive = false;
            } else if (currentDir != 0) {
                autoDrive = true;
                lastMoveTime = chrono::steady_clock::now();
            }
            needRedraw = true;
        } else if (key == 72 || key == 80 || key == 75 || key == 77) {
            currentDir = key; // remember the direction either way
            needRedraw = true;
            if (!autoDrive) {
                // Manual mode: move right away on key press.
                switch (key) {
                    case 72: newRow--; break;
                    case 80: newRow++; break;
                    case 75: newCol--; break;
                    case 77: newCol++; break;
                }
                playerMoved = true;
            }
        }

        if (autoDrive && !playerMoved) {
            auto now = chrono::steady_clock::now();
            double elapsed = chrono::duration<double>(now - lastMoveTime).count();
            if (elapsed >= moveIntervalSec) {
                lastMoveTime = now;
                switch (currentDir) {
                    case 72: newRow--; break; // Up
                    case 80: newRow++; break; // Down
                    case 75: newCol--; break; // Left
                    case 77: newCol++; break; // Right
                }
                playerMoved = true;
            }
        }

        // Random cars tick on the same interval as auto-drive, but on
        // their own clock -- they keep moving even if the player's own
        // auto-drive is off or hasn't started yet.
        if (!randomCars.empty()) {
            auto now = chrono::steady_clock::now();
            double elapsed = chrono::duration<double>(now - lastRandomMoveTime).count();
            if (elapsed >= moveIntervalSec) {
                lastRandomMoveTime = now;
                for (size_t i = 0; i < randomCars.size(); i++) {
                    moveRandomCar(randomCars[i], grid);
                }
                needRedraw = true;
            }
        }

        if (!playerMoved) {
            if (!needRedraw) Sleep(30); // avoid spinning the CPU while nothing happens
            continue;
        }

        needRedraw = true;

        if (!grid.isInBounds(newRow, newCol)) {
            cout << "Off the grid: crashed!\n";
            gameOver = true;
        } else if (grid.isWall(newRow, newCol)) {
            obstacles[0]->onHit();
            gameOver = true;
        } else {
            car.moveTo(newRow, newCol);
            if (checkWin(car, dest.first, dest.second)) {
                printGrid(grid, car, dest.first, dest.second, randomCars);
                cout << "You arrived!\n";
                gameOver = true;
            }
        }

        if (gameOver) {
            double totalSec = chrono::duration<double>(
                chrono::steady_clock::now() - startTime).count();
            cout << "Time: " << totalSec << " seconds\n";
        }
    }

    return 0;
}
