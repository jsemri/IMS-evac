#ifndef __evac_ca_h
#define __evac_ca_h

#include <vector>
#include <iostream>

class EvacCA {
private:
    // position in matrix
    using CellPosition = std::pair<size_t, size_t>;
    // type of a cell
    enum CellType {
        Empty = 0, Wall, Obstacle, Person, Exit};

    // cell structure
    struct Cell{
        CellType cell_type;
        int exit_distance;
        union {
            // only for empty cell
            // the higher number the higher priority
            int person_occurence_priority;
            // XXX maybe add more properties for other cell types
        };

        Cell() : cell_type{Empty}, exit_distance{-1} {}
    };

    // 2D matrix of cells
    std::vector<std::vector<Cell>> cells;
    // positions of the people to evacuate
    std::vector<CellPosition> people;

public:
    // XXX some additional parameters may be added later
    // considered parameters: chaos
    EvacCA(unsigned x, unsigned y);
    ~EvacCA() = default;

    bool evolve();
    void show() const;
    void add_people(int people);

    static EvacCA load_from_pixmap(const std::string &filename);
};

#endif
