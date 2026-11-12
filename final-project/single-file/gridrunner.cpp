// GridRunner - COMP-051 Semester Project
// Drive the car with the arrow keys from a random start to a random
// destination on a grid that fills the console window. Press A to
// toggle auto-drive: the car keeps moving in the last direction you
// picked, once per interval, without you pressing anything.
// Roads (not grass) are where you're allowed to drive; a speed road
// makes you move faster, a tunnel hides your car while you're on it,
// and two kinds of obstacles -- walls and potholes -- behave
// differently when you hit them. Press F to toggle weather fog
// (you can only see a radius around the car), and E to toggle emoji
// tiles. Random "parallel cars" (shown as R, or a car emoji) roam the
// roads on their own.
// Hit a wall or the edge and you lose -- press R at the end to try
// again with a brand new grid.
// Build in Visual Studio (Windows only - uses <conio.h> and <windows.h>).
//
// NOTE on emoji: the emoji glyphs below are written as raw UTF-8 byte
// escapes (\xNN...), not literal characters, specifically so they
// don't depend on the compiler's source-file encoding or any extra
// compiler flag -- they'll compile correctly in plain MSVC or g++
// either way. What they still depend on is the TERMINAL: you need a
// UTF-8-aware console with a color-emoji-capable font (Windows
// Terminal + a font like Segoe UI Emoji) for them to render as actual
// pictures. On an older console (classic cmd.exe) they may show as
// boxes or '?'. That's exactly why emoji mode defaults to OFF and can
// be toggled with E -- the color-coded ASCII tiles always work.
#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <algorithm>
#include <windows.h>
#include <conio.h>
using namespace std;

// ---- Tile types -------------------------------------------------------
// Every grid cell is exactly one of these. Roads (and the two special
// roads, speed/tunnel) are drivable; grass is not. Walls and potholes
// are obstacles that sit on a cell instead of a plain road.
const char GRASS = '.';
const char ROAD = 'o';
const char SPEED_ROAD = 's';
const char TUNNEL = 't';
const char WALL = '#';
const char POTHOLE = 'p';

// ---- Console colors -----------------------------------------------------
void setColor(int color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), (WORD)color);
}
const int COLOR_GRASS = 2;      // dark green
const int COLOR_ROAD = 8;       // dark gray
const int COLOR_SPEED = 11;     // bright cyan
const int COLOR_TUNNEL = 13;    // bright magenta
const int COLOR_WALL = 12;      // bright red
const int COLOR_POTHOLE = 6;    // brown/yellow
const int COLOR_CAR = 14;       // bright yellow
const int COLOR_DEST = 10;      // bright green
const int COLOR_RANDOMCAR = 9;  // bright blue
const int COLOR_FOG = 8;        // dark gray
const int COLOR_DEFAULT = 7;    // light gray (console default)

// ---- Sound --------------------------------------------------------------
void playCrashSound() { Beep(200, 300); }
void playTurnSound() { Beep(800, 60); }
void playPotholeSound() { Beep(400, 200); }

class Grid {
public:
    int rows, cols;
    vector<vector<char>> cells;

    Grid(int r, int c) : rows(r), cols(c), cells(r, vector<char>(c, GRASS)) {}

    bool isWall(int r, int c) const { return cells[r][c] == WALL; }
    bool isPothole(int r, int c) const { return cells[r][c] == POTHOLE; }
    bool isGrass(int r, int c) const { return cells[r][c] == GRASS; }
    bool isSpeedRoad(int r, int c) const { return cells[r][c] == SPEED_ROAD; }
    bool isTunnel(int r, int c) const { return cells[r][c] == TUNNEL; }

    // "Blocked" means a car can never enter this cell -- either it's
    // an obstacle (wall) or it's not a road at all (grass).
    bool isBlocked(int r, int c) const { return isWall(r, c) || isGrass(r, c); }

    bool isInBounds(int r, int c) const {
        return r >= 0 && r < rows && c >= 0 && c < cols;
    }

    // Lays down a real city grid: a straight horizontal street every
    // `spacing` rows and a straight vertical street every `spacing`
    // columns, crossing at right angles -- just like city blocks, with
    // grass filling the inside of each block.
    void buildCityStreets(int spacing) {
        for (int r = 0; r < rows; r++) {
            for (int c = 0; c < cols; c++) {
                if (r % spacing == 0 || c % spacing == 0) {
                    cells[r][c] = ROAD;
                }
            }
        }
    }

    vector<pair<int, int>> allRoadCells() const {
        vector<pair<int, int>> out;
        for (int r = 0; r < rows; r++) {
            for (int c = 0; c < cols; c++) {
                if (cells[r][c] == ROAD) out.push_back({ r, c });
            }
        }
        return out;
    }

    // Breadth-first search over drivable tiles only -- used to confirm
    // a candidate wall doesn't seal off the destination before we
    // actually commit to placing it.
    bool isReachable(int startR, int startC, int destR, int destC) const {
        vector<vector<bool>> visited(rows, vector<bool>(cols, false));
        vector<pair<int, int>> queue;
        queue.push_back({ startR, startC });
        visited[startR][startC] = true;
        size_t head = 0;
        int dr[4] = { -1, 1, 0, 0 };
        int dc[4] = { 0, 0, -1, 1 };
        while (head < queue.size()) {
            pair<int, int> cur = queue[head++];
            if (cur.first == destR && cur.second == destC) return true;
            for (int d = 0; d < 4; d++) {
                int nr = cur.first + dr[d];
                int nc = cur.second + dc[d];
                if (isInBounds(nr, nc) && !visited[nr][nc] && !isBlocked(nr, nc)) {
                    visited[nr][nc] = true;
                    queue.push_back({ nr, nc });
                }
            }
        }
        return false;
    }

    // Upgrades a few plain road tiles into speed roads / tunnels, and
    // drops a few obstacles -- all without ever touching the start or
    // destination cell. Every candidate wall is tried, then checked
    // with a BFS; it's only kept if the destination is still reachable
    // afterward, so the grid is always solvable no matter where a wall
    // lands in the street grid.
    void addRoadFeatures(int speedCount, int tunnelCount, int wallCount,
                          int potholeCount, int startR, int startC,
                          int destR, int destC) {
        auto randomRoadCell = [&]() {
            for (int attempt = 0; attempt < 500; attempt++) {
                int r = rand() % rows;
                int c = rand() % cols;
                if (cells[r][c] == ROAD &&
                    !(r == startR && c == startC) &&
                    !(r == destR && c == destC)) {
                    return make_pair(r, c);
                }
            }
            return make_pair(-1, -1); // no spot found; caller must check
        };

        for (int i = 0; i < speedCount; i++) {
            auto cell = randomRoadCell();
            if (cell.first != -1) cells[cell.first][cell.second] = SPEED_ROAD;
        }
        for (int i = 0; i < tunnelCount; i++) {
            auto cell = randomRoadCell();
            if (cell.first != -1) cells[cell.first][cell.second] = TUNNEL;
        }
        for (int i = 0; i < wallCount; i++) {
            for (int attempt = 0; attempt < 500; attempt++) {
                auto cell = randomRoadCell();
                if (cell.first == -1) break; // nowhere left to try
                cells[cell.first][cell.second] = WALL;
                if (isReachable(startR, startC, destR, destC)) break; // keep it
                cells[cell.first][cell.second] = ROAD; // revert, try a different cell
            }
        }
        for (int i = 0; i < potholeCount; i++) {
            auto cell = randomRoadCell(); // doesn't block movement, no BFS needed
            if (cell.first != -1) cells[cell.first][cell.second] = POTHOLE;
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

// A pothole doesn't end the game -- it just costs you control of the
// car for a second while it "gets stuck."
class Pothole : public Obstacle {
public:
    void onHit() override {
        cout << "Stuck in a pothole! Lost a second.\n";
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

// Background tiles (grass/road/speed/tunnel/wall/pothole) with no car or
// destination on them. Background tiles are always a real visible glyph,
// never a blank space -- a blank space colored a certain way is still
// invisible, so "the road network" wouldn't actually show up on screen.
void printTile(char tile, bool emojiMode) {
    switch (tile) {
        case GRASS:
            setColor(COLOR_GRASS);
            cout << ". ";
            break;
        case SPEED_ROAD:
            setColor(COLOR_SPEED);
            // Every tile -- ASCII or emoji -- prints as exactly 2 output
            // characters, glyph then a trailing space. Relying on an
            // emoji rendering "double-wide" in the terminal instead of
            // padding it ourselves is what let road/wall/etc. tiles
            // drift narrower than grass tiles and throw the whole row
            // out of alignment (reported: streets visibly stopping
            // partway across, grass leaking in on the right).
            if (emojiMode) cout << "\xE2\x9A\xA1\xEF\xB8\x8F "; // ⚡️
            else cout << "s ";
            break;
        case TUNNEL:
            setColor(COLOR_TUNNEL);
            if (emojiMode) cout << "\xF0\x9F\x9A\x87 "; // 🚇
            else cout << "t ";
            break;
        case WALL:
            setColor(COLOR_WALL);
            if (emojiMode) cout << "\xF0\x9F\xA7\xB1 "; // 🧱
            else cout << "# ";
            break;
        case POTHOLE:
            setColor(COLOR_POTHOLE);
            if (emojiMode) cout << "\xF0\x9F\x95\xB3 "; // 🕳
            else cout << "p ";
            break;
        default: // ROAD
            setColor(COLOR_ROAD);
            // A double horizontal line reads as a two-lane road (the
            // same way the real motorway emoji below already does).
            if (emojiMode) cout << "\xF0\x9F\x9B\xA3\xEF\xB8\x8F "; // 🛣️
            else cout << "\xE2\x95\x90\xE2\x95\x90"; // ══ (box-drawing double line)
            break;
    }
    setColor(COLOR_DEFAULT);
}

void printGrid(const Grid& grid, const Car& car, int destRow, int destCol,
               const vector<Car>& randomCars, bool fogOn, int fogRadius,
               bool emojiMode) {
    for (int r = 0; r < grid.rows; r++) {
        for (int c = 0; c < grid.cols; c++) {
            // Weather fog: anything outside the car's visible radius is
            // hidden, no matter what's actually there.
            if (fogOn) {
                int dr = abs(r - car.row);
                int dc = abs(c - car.col);
                if (max(dr, dc) > fogRadius) {
                    // A hidden cell is a blank placeholder -- not a '?' or
                    // a cloud glyph, just empty space -- so fog genuinely
                    // reads as "nothing known is here" instead of drawing
                    // attention to itself.
                    cout << "  ";
                    continue;
                }
            }

            bool isRandomCar = false;
            for (size_t i = 0; i < randomCars.size(); i++) {
                if (randomCars[i].row == r && randomCars[i].col == c) {
                    isRandomCar = true;
                    break;
                }
            }

            bool isPlayerHere = (r == car.row && c == car.col);
            bool inTunnel = grid.isTunnel(r, c);

            // A car hiding in a tunnel is NOT drawn -- the tile itself
            // still shows (so the player can see a tunnel is there),
            // but the car marker disappears while it's inside one.
            if (isPlayerHere && !inTunnel) {
                setColor(COLOR_CAR);
                cout << (emojiMode ? "\xF0\x9F\x9A\x97" : "C "); // 🚗
            } else if (r == destRow && c == destCol) {
                setColor(COLOR_DEST);
                cout << (emojiMode ? "\xF0\x9F\x8F\x81" : "D "); // 🏁
            } else if (isRandomCar && !inTunnel) {
                setColor(COLOR_RANDOMCAR);
                cout << (emojiMode ? "\xF0\x9F\x9A\x99" : "R "); // 🚙
            } else {
                printTile(grid.cells[r][c], emojiMode);
                continue;
            }
            setColor(COLOR_DEFAULT);
        }
        cout << "\n";
    }
}

bool checkWin(const Car& car, int destRow, int destCol) {
    return car.row == destRow && car.col == destCol;
}

// Picks one of the 4 directions at random and moves the car there if
// it's a legal move (on a road, not grass or a wall). If the random
// pick is off-road or off the grid, the car just stays put for this
// tick and tries again next time.
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

    if (grid.isInBounds(newRow, newCol) && !grid.isBlocked(newRow, newCol)) {
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
    SetConsoleOutputCP(CP_UTF8); // so the emoji byte escapes above render as UTF-8

    cout << "Enter auto-drive interval in seconds (e.g. 1.0): ";
    double moveIntervalSec = 1.0;
    cin >> moveIntervalSec;
    if (moveIntervalSec <= 0) moveIntervalSec = 1.0;

    cout << "How many random parallel cars? (0 for none): ";
    int numRandomCars = 0;
    cin >> numRandomCars;
    if (numRandomCars < 0) numRandomCars = 0;

    bool emojiMode = false; // off by default -- see the note at the top of this file
    bool playAgain = true;

    while (playAgain) {
        int consoleCols, consoleRows;
        getConsoleSize(consoleCols, consoleRows);

        // Leave a few lines below the grid for the status line and the
        // win/lose message, and one less column so the last cell doesn't
        // land exactly on the console's auto-wrap edge. Each printed cell
        // is 2 characters wide (see printTile/printGrid), so divide the
        // usable width by 2 to get the actual column count.
        int availCols = (consoleCols - 1) / 2;
        int availRows = consoleRows - 4;

        try {
            if (availCols < 5 || availCols > 50) throw BadGridSize();
            if (availRows < 5 || availRows > 50) throw BadGridSize();
        } catch (BadGridSize& e) {
            cout << e.what() << " -- clamping grid to fit.\n";
            if (availCols < 5) availCols = 5;
            if (availCols > 50) availCols = 50;
            if (availRows < 5) availRows = 5;
            if (availRows > 50) availRows = 50;
        }

        // A console window is almost never exactly as many rows tall as
        // it is "cell-columns" wide, so using availCols/availRows as-is
        // produces a rectangle, not a square. Force an actual N x N grid
        // by taking the smaller of the two -- that's the only way to
        // guarantee square regardless of the terminal's real shape.
        int gridSize = min(availCols, availRows);
        int gridCols = gridSize;
        int gridRows = gridSize;

        Grid grid(gridRows, gridCols);

        // A real city grid: straight streets every 4 cells, crossing at
        // right angles, with grass filling the blocks in between.
        const int STREET_SPACING = 4;
        grid.buildCityStreets(STREET_SPACING);

        vector<pair<int, int>> roadCells = grid.allRoadCells();
        pair<int, int> start = roadCells[rand() % roadCells.size()];
        pair<int, int> dest = roadCells[rand() % roadCells.size()];
        while (dest == start) dest = roadCells[rand() % roadCells.size()];

        Car car;
        car.moveTo(start.first, start.second);

        grid.addRoadFeatures(/*speed*/3, /*tunnel*/2, /*walls*/4, /*potholes*/3,
                              start.first, start.second, dest.first, dest.second);

        Wall wall;
        Pothole pothole;
        vector<Obstacle*> obstacles;
        obstacles.push_back(&wall);    // obstacles[0] = crash
        obstacles.push_back(&pothole); // obstacles[1] = stall

        vector<Car> randomCars(numRandomCars);
        for (int i = 0; i < numRandomCars; i++) {
            pair<int, int> spot;
            do {
                spot = randomCell(gridRows, gridCols);
            } while (grid.isBlocked(spot.first, spot.second) ||
                     spot == start || spot == dest);
            randomCars[i].moveTo(spot.first, spot.second);
        }

        bool gameOver = false;
        bool autoDrive = false;
        bool fogOn = false;
        int fogRadius = 5;
        int currentDir = 0; // last direction picked; 0 = none yet
        auto lastMoveTime = chrono::steady_clock::now();        // player's own auto-drive tick
        auto lastRandomMoveTime = chrono::steady_clock::now();  // random cars' tick
        auto startTime = chrono::steady_clock::now();           // whole-run timer
        auto stalledUntil = chrono::steady_clock::now();        // not stalled yet
        int lastShownSec = -1;

        bool needRedraw = true; // only redraw when something actually changed

        while (!gameOver) {
            // The clock itself ticks every loop (not just on needRedraw), so the
            // on-screen timer updates every second even while nothing else changed.
            int elapsedWhole = (int)chrono::duration<double>(
                chrono::steady_clock::now() - startTime).count();
            if (elapsedWhole != lastShownSec) {
                lastShownSec = elapsedWhole;
                needRedraw = true;
            }

            if (needRedraw) {
                system("cls");
                printGrid(grid, car, dest.first, dest.second, randomCars,
                          fogOn, fogRadius, emojiMode);
                setColor(COLOR_DEFAULT);
                cout << "Time: " << elapsedWhole << "s | ";

                bool isStalledNow = chrono::steady_clock::now() < stalledUntil;
                if (isStalledNow) {
                    setColor(COLOR_POTHOLE);
                    cout << "STUCK! ";
                    setColor(COLOR_DEFAULT);
                } else if (grid.isSpeedRoad(car.row, car.col)) {
                    setColor(COLOR_SPEED);
                    cout << "SPEED ROAD! ";
                    setColor(COLOR_DEFAULT);
                } else if (grid.isTunnel(car.row, car.col)) {
                    setColor(COLOR_TUNNEL);
                    cout << "Hidden in a tunnel! ";
                    setColor(COLOR_DEFAULT);
                }

                cout << "Arrows move. A = auto-drive (" << moveIntervalSec << "s), "
                     << (autoDrive ? "ON" : "OFF") << " " << directionName(currentDir)
                     << " | F = fog (" << (fogOn ? "ON" : "OFF")
                     << "), E = emoji (" << (emojiMode ? "ON" : "OFF") << ")";
                if (!autoDrive && currentDir == 0) {
                    cout << " (press an arrow first, then A)";
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
            } else if (key == 'f' || key == 'F') {
                fogOn = !fogOn;
                needRedraw = true;
            } else if (key == 'e' || key == 'E') {
                emojiMode = !emojiMode;
                needRedraw = true;
            } else if (key == 72 || key == 80 || key == 75 || key == 77) {
                if (key != currentDir) playTurnSound(); // beep only on an actual change
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
                // Standing on a speed road halves the wait between auto-drive moves.
                double interval = grid.isSpeedRoad(car.row, car.col)
                                       ? moveIntervalSec / 2.0 : moveIntervalSec;
                if (elapsed >= interval) {
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

            // A pothole stall overrides everything above -- no key press or
            // auto-drive tick can move the car while it's stuck.
            bool isStalled = chrono::steady_clock::now() < stalledUntil;
            if (isStalled) playerMoved = false;

            // Random cars tick on the same interval as auto-drive, but on
            // their own clock -- they keep moving even if the player's own
            // auto-drive is off, stalled, or hasn't started yet.
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
                playCrashSound();
                gameOver = true;
            } else if (grid.isWall(newRow, newCol)) {
                obstacles[0]->onHit();
                playCrashSound();
                gameOver = true;
            } else if (grid.isGrass(newRow, newCol)) {
                cout << "Roads only -- can't drive on grass!\n";
                Sleep(400); // brief pause so the message is actually readable
            } else if (grid.isPothole(newRow, newCol)) {
                car.moveTo(newRow, newCol);
                obstacles[1]->onHit();
                playPotholeSound();
                stalledUntil = chrono::steady_clock::now() + chrono::milliseconds(1000);
            } else {
                car.moveTo(newRow, newCol);
                if (checkWin(car, dest.first, dest.second)) {
                    printGrid(grid, car, dest.first, dest.second, randomCars,
                              fogOn, fogRadius, emojiMode);
                    setColor(COLOR_DEFAULT);
                    cout << "You arrived!\n";
                    gameOver = true;
                }
            }

            if (gameOver) {
                double totalSec = chrono::duration<double>(
                    chrono::steady_clock::now() - startTime).count();
                setColor(COLOR_DEFAULT);
                cout << "Time: " << totalSec << " seconds\n";
            }
        }

        cout << "\nPress R to try again, or any other key to quit...";
        int choice = _getch();
        if (choice == 224) _getch(); // swallow an arrow key's second byte, if pressed
        playAgain = (choice == 'r' || choice == 'R');
        if (playAgain) cout << "\n";
    }

    setColor(COLOR_DEFAULT);
    return 0;
}
