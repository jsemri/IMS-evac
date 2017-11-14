#pragma GCC diagnostic ignored "-Wsign-compare"

#include <iostream>
#include <algorithm>
#include <ctime>
#include <cstdlib>
#include <cassert>

#include "bitmap_image.hpp"
#include "evac_ca.h"

EvacCA::EvacCA(unsigned x, unsigned y) :
    cells(y, std::vector<Cell>(x))
{

}

std::vector<EvacCA::CellPosition>
EvacCA::cell_neighbourhood(CellPosition position) const {
    std::vector<CellPosition> neighbours;
    size_t row = position.first;
    size_t col = position.second;
    size_t max_row = cells.size();
    size_t max_col = cells[0].size();

    if (row != 0) {
        push_if_empty(neighbours, row - 1, col);
    }

    if (row != max_row) {
        push_if_empty(neighbours, row + 1, col);
    }

    if (col != 0) {
        push_if_empty(neighbours, row, col - 1);
    }

    if (col != max_col) {
        push_if_empty(neighbours, row, col + 1);
    }

    if (col != 0 && row != 0) {
        push_if_empty(neighbours, row - 1, col - 1);
        push_if_empty(neighbours, row - 1, col - 1);
    }

    if (col != max_col && row != max_row) {
        push_if_empty(neighbours, row + 1, col + 1);
        push_if_empty(neighbours, row + 1, col + 1);
    }

    return neighbours;
}

bool EvacCA::evolve() {

    bool state_change = false;
    std::random_shuffle(
        people.begin(), people.end(),
        [](int i) { return std::rand() % i;});

    for (auto &i : people) {

        if (get_cell(i).evacuated) {
            // already evacuated
            continue;
        }

        state_change = true;

        auto neighbours = cell_neighbourhood(i);
        // find min value
        if (!neighbours.empty()) {
            auto dist = *std::min_element(
                neighbours.begin(), neighbours.end(),
                [this](const CellPosition a, const CellPosition b) {
                    return distance(a) < distance(b);
                });

            if (distance(dist) < distance(i)) {
                // move the person
                get_cell(i) = Cell();
                i = dist;
                get_cell(i).cell_type = Person;
            }
            // TODO else if chaos

            // if value is zero -> leave the system
            if (distance(i) == 0) {
                // TODO erase from vector? Label as evacuated?
                get_cell(i).evacuated = true;
            }
        }
    }

    return false; // TODO remove
    return state_change;
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
   	// Open file
	bitmap_image image(filename);
	if(!image) {
		// Could not open file
		throw std::invalid_argument("could not open input file");
	}

	// Read dimensions
	int width = image.width();
	int height = image.height();
	EvacCA ca(height, width);
	
	// Read colors
	for(int row = 0; row < height; row++) {
		for(int col = 0; col < width; col++) {
			// Differentiate colors
			unsigned char r, g, b;
			image.get_pixel(row, col, r, g, b);
			CellType type;
			if(r == 255 && g == 255 && b == 255) {
				// Wall is black
				type = Wall;
			} else if(r == 255 && g == 0 && b == 0) {
				// Exit is red
				type = Exit;
			} else {
				// Everything else is empty
				type = Empty;
			}
			ca.cells[row][col].cell_type = type;
		}
	}
	
	// Success
	return ca;
}

void EvacCA::show() const {
    // TODO
    // output CA as an image
}

/**
 * Simple model creation: four walls and one exit.
 * @param wall length
 *
static void sample(int length) {
	// Set dimensions
	int width = length, height = length;
	
	// Create image
	bitmap_image image(width, height);
	
	// White background
	image.set_all_channels(255, 255, 255);
	
	// Set drawer
	image_drawer draw(image);
	draw.pen_width(1);
	
	// Draw walls
	draw.pen_color(0, 0, 0);
	draw.horiztonal_line_segment(0, width, 0);
	draw.horiztonal_line_segment(0, width, height-1);
	draw.vertical_line_segment(0, height, 0);
	draw.vertical_line_segment(0, height, height-1);
	
	// Draw exit
	draw.pen_color(255,0,0);
	draw.plot_pixel(width/2,0);
	
	// Output
	image.save_image("output.bmp");
}
**/
