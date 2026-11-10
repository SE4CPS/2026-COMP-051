// GridRunner - COMP-051 Semester Project
// Drive the car with the arrow keys from a random start to a random
// destination on a customizable grid. Hit a wall or the edge and you lose.
// Build in Visual Studio (Windows only - uses <conio.h>).
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <conio.h>
using namespace std;

const int SIZE = 20;

class Grid {
public:
    int size;
    vector<vector<char>> cells;

    Grid(int s) : size(s), cells(s, vector<char>(s, '.')) {}

    bool isWall(int r, int c) const {
        return cells[r][c] == '#';
    }

    bool isInBounds(int r, int c) const {
        return r >= 0 && r < size && c >= 0 && c < size;
    }

    void addRandomWalls(int count) {
        for (int i = 0; i < count; i++) {
            int r = rand() % size;
            int c = rand() % size;
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

pair<int, int> randomCell(int size) {
    return { rand() % size, rand() % size };
}

void printGrid(const Grid& grid, const Car& car, int destRow, int destCol) {
    for (int r = 0; r < grid.size; r++) {
        for (int c = 0; c < grid.size; c++) {
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

int main() {
    srand((unsigned)time(0));

    try {
        if (SIZE < 5 || SIZE > 50) throw BadGridSize();
    } catch (BadGridSize& e) {
        cout << e.what() << "\n";
        return 1;
    }

    Grid grid(SIZE);
    grid.addRandomWalls(SIZE * 2);

    Car car;
    pair<int, int> start = randomCell(SIZE);
    car.moveTo(start.first, start.second);

    pair<int, int> dest = randomCell(SIZE);
    while (dest == start) dest = randomCell(SIZE);

    vector<Obstacle*> obstacles;
    Wall wall;
    obstacles.push_back(&wall);

    bool gameOver = false;
    cout << "GridRunner: use the arrow keys. C = you, D = destination.\n";

    while (!gameOver) {
        system("cls");
        printGrid(grid, car, dest.first, dest.second);

        int newRow = car.row;
        int newCol = car.col;

        int key = _getch();
        if (key == 224) key = _getch();
        switch (key) {
            case 72: newRow--; break; // Up
            case 80: newRow++; break; // Down
            case 75: newCol--; break; // Left
            case 77: newCol++; break; // Right
        }

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
