#include "evac_ca.h"

EvacCA::EvacCA(unsigned x, unsigned y) :
    cells(y, std::vector<EvacCACell>(x))
{

}

bool EvacCA::evolve() {
    // TODO
    return false;
}

void EvacCA::show() const {
    // TODO
    // output CA as an image
}

void EvacCA::add_people(int people) {
    // TODO
    // somehow distribute people over empty cells
}

EvacCA EvacCA::load_from_pixmap(const std::string &filename) {
    // TODO
    // load CA from an image
    return EvacCA(1000, 1000);
}
