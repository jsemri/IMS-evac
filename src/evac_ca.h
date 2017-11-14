#ifndef __evac_ca_h
#define __evac_ca_h

#include <vector>
#include <iostream>

class EvacCA {
private:
    enum EvacCACellType {Empty = 0, Wall, Obstacle , Person, Exit};

    struct EvacCACell{
        EvacCACellType cell_type;
        int exit_distance;

        EvacCACell() : cell_type{Empty}, exit_distance{-1} {}
    };

    std::vector<std::vector<EvacCACell>> cells;

public:
    // XXX some additional parameters may be added later
    EvacCA(unsigned x, unsigned y);
    ~EvacCA() = default;

    bool evolve();
    void show() const;
    void add_people(int people);

    static EvacCA load_from_pixmap(const std::string &filename);
};

#endif
