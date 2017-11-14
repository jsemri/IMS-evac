#ifndef __evac_ca_h
#define __evac_ca_h

#include <vector>
#include <iostream>

// type of a cell
enum CellType {
    Empty = 0, Wall, Obstacle, Person, Exit
};

class EvacCA {
public:
    // cell structure
    struct Cell {
        CellType cell_type;
        int exit_distance;
        union {
            // only for empty cell
            // the higher number the higher priority
            int person_occurence_priority;
            // only for person
            bool evacuated;
            // XXX maybe add more properties for other cell types
        };

        Cell() : cell_type{Empty}, exit_distance{-1} {}
    };


private:
    // position in matrix
    using CellPosition = std::pair<size_t, size_t>;

    // 2D matrix of cells
    std::vector<std::vector<Cell>> cells;
    // positions of the people to evacuate
    std::vector<CellPosition> people;

    std::vector<CellPosition> cell_neighbourhood(CellPosition position) const;

    // inline methods

    /// returns true if cell at specified position is empty or it is an exit
    inline bool is_empty(size_t row, size_t col) const {
        return cells[row][col].cell_type == Empty ||
               cells[row][col].cell_type == Exit; // XXX Exit?
    };

    /// returns true if cell at specified position is empty or it is an exit
    inline void push_if_empty(
        std::vector<CellPosition> &vec, size_t row, size_t col) const
    {
        if (is_empty(row, col)) {
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

    /// return a reference to a cell at specified position
    inline Cell& get_cell(CellPosition pos) {
        return cells[pos.first][pos.second];
    }

public:
    // XXX some additional parameters may be added later
    // considered parameters: chaos
    EvacCA(unsigned x, unsigned y);
    ~EvacCA() = default;

    bool evolve();
    void add_people(int people);

    void show() const;
    static EvacCA load_from_pixmap(const std::string &filename);

    // inline methods
    inline Cell &get_cell(int row, int col) {
        return cells[row][col];
    }
    inline int height() const {
        return cells.size();
    }
    inline int width() const {
        return cells[0].size();
    }
};

#endif
