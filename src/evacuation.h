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
    Empty =            0b0000000001,
    Exit =             0b0000000010,
    Wall =             0b0000000100,
    Obstacle =         0b0000001000,
    Person =           0b0000010000,
    Smoke =            0b0000100000,
    PersonAppearance = 0b0001000000,
    PersonAtExit =     0b0010000000,
    PersonWithSmoke =  0b0100000000,
    ObstacleWithSmoke =0b1000000000
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
    static constexpr int EmptyCells = Empty | Smoke | PersonAppearance | Exit;
    static constexpr int SmokeCells = Smoke | ObstacleWithSmoke
                                            | PersonWithSmoke;
    /// 2D matrix of cells.
    std::vector<std::vector<Cell>> cells;

    /// variables for computing some statistics
    int pedestrians;
    int time;
    int casualties;
    int moves;

    // methods

    /// Return Moore neighbourhood of cells of specified type at current
    /// position. Default returned cell types are defined above.
    std::vector<CellPosition> cell_neighbourhood(
        CellPosition position, int cell_types = EmptyCells) const;
    std::vector<CellPosition> cell_neighbourhood(
        size_t row, size_t col, int cell_types = EmptyCells) const;

    /// Recompute exit distances.
    void recompute_shortest_paths();

    // Inline methods:

    /// returns true if cell coordinates are valid
    inline bool cell_check(int row, int col) const {
        return row >= 0 && row < height() && col >= 0 && col < width();
    }

    /// push an position(row,col) to the vector if cell at this position is
    /// empty
    inline void push_if(
        std::vector<CellPosition> &vec, size_t row, size_t col,
        int cell_types) const
    {
        if (cell_check(row, col) && (cells[row][col].type & cell_types)) {
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
