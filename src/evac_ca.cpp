#pragma GCC diagnostic ignored "-Wsign-compare"

#include <iostream>
#include <algorithm>
#include <ctime>
#include <cstdlib>
#include <cassert>

#include "evac_ca.h"

EvacCA::EvacCA(unsigned x, unsigned y) :
    cells(y, std::vector<Cell>(x))
{

}

bool EvacCA::evolve() {
    // TODO
    return false;
}

// somehow distribute people over empty cells
void EvacCA::add_people(int people_count) {

    std::vector<CellPosition> empty_cells;
    std::vector<CellPosition> empty_priority_cells;
    // mark all cells with possible person appearance
    for (size_t i = 0; i < cells.size(); i++) {
        for (size_t j = 0; j < cells[i].size(); j++) {
            if (cells[i][j].cell_type == Empty) {
                if (cells[i][j].person_occurence_priority > 0) {
                    empty_priority_cells.push_back(CellPosition(i,j));
                }
                else {
                    empty_cells.push_back(CellPosition(i,j));
                }
            }
        }
    }

    if (people_count > empty_cells.size() + empty_priority_cells.size()) {
        throw std::logic_error("cannot have more people than empty cells");
    }

    std::srand(std::time(0));
    std::random_shuffle(
        empty_priority_cells.begin(),
        empty_priority_cells.end(),
        [](int i) { return std::rand() % i;});

    std::random_shuffle(
        empty_cells.begin(), empty_cells.end(),
        [](int i) { return std::rand() % i;});

    // placing people according to priority
    while (!empty_priority_cells.empty() && people_count-- > 0) {
//        std::cerr << empty_priority_cells.back().first << "\n";
        people.push_back(empty_priority_cells.back());
        empty_priority_cells.pop_back();
    }

    while (people_count-- > 0) {
        assert(!empty_cells.empty());
//        std::cerr << empty_cells.back().first << "\n";
        people.push_back(empty_cells.back());
        empty_cells.pop_back();
    }
}

EvacCA EvacCA::load_from_pixmap(const std::string &filename) {
    // TODO
    // load CA from an image
    return EvacCA(1000, 1000);
}

void EvacCA::show() const {
    // TODO
    // output CA as an image
}

