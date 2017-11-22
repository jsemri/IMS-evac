#ifndef __evacuation_h
#define __evacuation_h

#include <vector>
#include <list>
#include <iostream>
#include <climits>
#include <cassert>

namespace Evacuation {

/// Position in matrix.
using CellPosition = std::pair<size_t, size_t>;

/** Type of a cell. */
enum CellType {
    Empty = 0,
    Exit,
    Wall,
    Obstacle,
    Person,
    Smoke,
    PersonAppearance,
    PersonAtExit,
    SmokeWithPerson,
    SmokeWithObstacle
};

/** Cell structure. */
struct Cell {
    /** Cell type. */
    CellType type;
    /** Distance to upper border. */
    int row;
    /** Distance to left border. */
    int col;
    /** Distance (in hops) to nearest exit. */
    unsigned exit_distance;
    // XXX maybe add more properties for other cell types

    Cell() :
        type{Empty}, row{-1}, col{-1}, exit_distance{UINT_MAX}
    {}
};

class CA {
public:
    CA(unsigned height, unsigned width);
    ~CA() = default;

    /// Apply transition function on CA states.
    /// @return false if there are no people to evacuate, true otherwise
    bool evolve();

    /// Distribute people over the building.
    void add_people(int people);

    /// Distribute smoke over the building.
    void add_smoke(int smoke);

    /**
     * Load model description from a bitmap.
     * @param filename name of input file
     * @return instance of CA class
     * @throw invalid_argument if failed to process input file
     * @note loaded model might be populated
     */
    static CA load(const std::string &filename);

    /// Store model description to "output.bmp".
    void show();

    void print_statistics() const noexcept;

    // Inline methods:

    /// Dimension getter: number of rows
    inline int height() const {
        return cells.size();
    }

    /// Dimension getter: number of columns
    inline int width() const {
        assert(!cells.empty());
        return cells[0].size();
    }

    /// Retrieve a cell at a specified position
    inline Cell& cell(int row, int col) {
        return cells[row][col];
    }

private:
    /// 2D matrix of cells.
    std::vector<std::vector<Cell>> cells;

    /// variables for computing some statistics
    int pedestrians;
    int time;
    int casualties;
    int moves;
    int max_distance;

    // methods

    /// Return Moore neighbourhood of empty cells at current position.
    std::vector<CellPosition> cell_neighbourhood(CellPosition position) const;
    std::vector<CellPosition> cell_neighbourhood(size_t row, size_t col) const;
    std::vector<CellPosition> cell_neighbourhood2(size_t row, size_t col) const;

    /// Recompute exit distances.
    void recompute_shortest_paths();

    /// Compute max distance.
    void compute_max_distance();

    // Inline methods:

    /// returns true if cell at specified position is empty or it is an exit
    inline bool is_empty(size_t row, size_t col) const {
        return cells[row][col].type == Empty ||
               cells[row][col].type == PersonAppearance ||
               cells[row][col].type == Smoke ||
               cells[row][col].type == Exit;
    };

    /// returns true if cell at specified position is empty or it is an exit
    inline bool is_empty2(size_t row, size_t col) const {
        return cells[row][col].type == Empty ||
               cells[row][col].type == PersonAppearance ||
               cells[row][col].type == Smoke ||
               cells[row][col].type == SmokeWithObstacle ||
               cells[row][col].type == Exit;
    };

    /// returns true if cell coordinates are valid
    inline bool cell_check(int row, int col) const {
        return row >= 0 && row < height() && col >= 0 && col < width();
    }

    /// push an position(row,col) to the vector if cell at this position is
    /// empty
    inline void push_if_empty(
        std::vector<CellPosition> &vec, size_t row, size_t col) const
    {
        if (cell_check(row, col) && is_empty(row, col)) {
            vec.push_back(CellPosition(row, col));
        }
    }

    inline void push_if_empty2(
        std::vector<CellPosition> &vec, size_t row, size_t col) const
    {
        if (cell_check(row, col) && is_empty2(row, col)) {
            vec.push_back(CellPosition(row, col));
        }
    }

    /// return a distance to exit from cell at specified position
    inline int distance(CellPosition pos) const {
        return distance(pos.first, pos.second);
    }

    /// return a distance to exit from cell at specified position
    inline int distance(size_t row, size_t col) const {
        return cells[row][col].exit_distance;
    }

    /// Retrieve a cell at a specified position
    inline Cell& cell(CellPosition pos) {
        return cell(pos.first, pos.second);
    }
};

} // end of namespace

#endif
