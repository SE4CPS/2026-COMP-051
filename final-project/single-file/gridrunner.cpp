// GridRunner - COMP-051 Semester Project
// Drive the car with the arrow keys from a random start to a random
// destination on a grid that fills the console window. Press A to
// toggle auto-drive: the car keeps moving in the last direction you
// picked, once per interval, without you pressing anything.
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

void printGrid(const Grid& grid, const Car& car, int destRow, int destCol) {
    for (int r = 0; r < grid.rows; r++) {
        for (int c = 0; c < grid.cols; c++) {
            if (r == car.row && c == car.col) cout << 'C';
            else if (r == destRow && c == destCol) cout << 'D';
            else cout << grid.cells[r][c];
        }
        cout << "\n";
    }
}

bool checkWin(const Car& car, int destRow, int destCol) {
    return car.row == destRow && car.col == destCol;
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
    // land exactly on the console's auto-wrap edge.
    int gridCols = consoleCols - 1;
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

    bool gameOver = false;
    bool autoDrive = false;
    int currentDir = 0; // last direction picked; 0 = none yet
    auto lastMoveTime = chrono::steady_clock::now();

    bool needRedraw = true; // only redraw when something actually changed

    while (!gameOver) {
        if (needRedraw) {
            system("cls");
            printGrid(grid, car, dest.first, dest.second);
            cout << "Arrows move. A = toggle auto-drive (" << moveIntervalSec << "s). "
                 << "Auto-drive: " << (autoDrive ? "ON" : "OFF")
                 << ", direction: " << directionName(currentDir) << "\n";
            needRedraw = false;
        }

        int newRow = car.row;
        int newCol = car.col;
        int key = -1;

        if (autoDrive) {
            // Non-blocking check: the car should keep moving on its own
            // even if the player doesn't press anything this frame.
            if (_kbhit()) {
                key = _getch();
                if (key == 224) key = _getch();
            }

            if (key == 'a' || key == 'A') {
                autoDrive = false;
                needRedraw = true;
                continue;
            }
            if (key == 72 || key == 80 || key == 75 || key == 77) {
                currentDir = key; // change direction; still waits for the next tick
                needRedraw = true; // update the status line's direction label now
            }

            auto now = chrono::steady_clock::now();
            double elapsed = chrono::duration<double>(now - lastMoveTime).count();
            if (elapsed < moveIntervalSec) {
                Sleep(30); // small pause so we don't spin the CPU while waiting
                continue;
            }
            lastMoveTime = now;
            key = currentDir;
        } else {
            key = _getch();
            if (key == 224) key = _getch();

            if (key == 'a' || key == 'A') {
                if (currentDir != 0) {
                    autoDrive = true;
                    lastMoveTime = chrono::steady_clock::now();
                }
                needRedraw = true;
                continue;
            }
        }

        switch (key) {
            case 72: newRow--; currentDir = 72; break; // Up
            case 80: newRow++; currentDir = 80; break; // Down
            case 75: newCol--; currentDir = 75; break; // Left
            case 77: newCol++; currentDir = 77; break; // Right
            default: continue; // unrecognized key, nothing changed
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
                printGrid(grid, car, dest.first, dest.second);
                cout << "You arrived!\n";
                gameOver = true;
            }
        }
    }

    return 0;
}
