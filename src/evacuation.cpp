#pragma GCC diagnostic ignored "-Wsign-compare"

#include <iostream>
#include <algorithm>
#include <ctime>
#include <cstdlib>
#include <cassert>

#include <climits>
#include <queue>

#include "evacuation.h"
#include "bitmap.h"

using namespace Evacuation;

CA::CA(unsigned y, unsigned x) :
    cells(y, std::vector<Cell>(x))
{}

std::vector<CA::CellPosition>
CA::cell_neighbourhood(CellPosition position) const {
    std::vector<CellPosition> neighbours;
    size_t row = position.first;
    size_t col = position.second;

	push_if_empty(neighbours, row - 1, col - 1);
	push_if_empty(neighbours, row - 1, col);
	push_if_empty(neighbours, row - 1, col + 1);
	push_if_empty(neighbours, row, col - 1);
	push_if_empty(neighbours, row, col + 1);
	push_if_empty(neighbours, row + 1, col - 1);
	push_if_empty(neighbours, row + 1, col);
	push_if_empty(neighbours, row + 1, col + 1);

    return neighbours;
}

bool CA::evolve() {

    bool res = !people.empty();
    // remove people at exits
    for (auto i : exits) {
        cell(i).type = Exit;
    }

    std::vector<size_t> to_erase;

    std::random_shuffle(
        people.begin(), people.end(),
        [](int i) { return std::rand() % i;});

    for (size_t i = 0; i < people.size(); i++) {
        auto &person = people[i];
        auto neighbours = cell_neighbourhood(person);
        // find min value
        if (!neighbours.empty()) {
            auto next_cell = *std::min_element(
                neighbours.begin(), neighbours.end(),
                [this](const CellPosition a, const CellPosition b) {
                    return distance(a) < distance(b);
                });

            if (distance(next_cell) < distance(person)) {
                // move the person
                cell(person).type = Empty;
                person = next_cell;
                cell(person).type = Person;
            }
            // TODO else if chaos

            if (distance(person) == 0) {
                to_erase.push_back(i);
            }
        }
    }

    // remove evacuated people
    int idx = 0;
    for (auto i : to_erase) {
        people.erase(people.begin()+i-idx);
        idx++;
    }

    return res;
}

// somehow distribute people over empty cells
void CA::add_people(int people_count) {

    std::vector<CellPosition> empty_cells;
    std::vector<CellPosition> empty_priority_cells;
    // mark all cells with possible person appearance
    for (size_t i = 0; i < height(); i++) {
        for (size_t j = 0; j < width(); j++) {
            if (cells[i][j].type == Empty) {
                empty_cells.push_back(CellPosition(i,j));
            }
            else if (cells[i][j].type == PersonAppearance) {
                empty_priority_cells.push_back(CellPosition(i,j));
            }
            else if (cells[i][j].type == Exit) {
                exits.push_back(CellPosition(i,j));
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
        people.push_back(empty_priority_cells.back());
        empty_priority_cells.pop_back();
    }

    while (people_count-- > 0) {
        assert(!empty_cells.empty());
        people.push_back(empty_cells.back());
        empty_cells.pop_back();
    }

    for (auto i : people) {
        cell(i).type = Person;
    }
}

void CA::recompute_shortest_paths() {
	// Identify exit states
   	std::queue<CellPosition> exit_states;
   	for (int row = 0; row < height(); row++) {
        for (int col = 0; col < width(); col++) {
			Cell &c = cell(row, col);
			if(c.type == Exit) {
				c.exit_distance = 0;
				exit_states.push(CellPosition(row, col));
			} else {
				c.exit_distance = UINT_MAX;
			}
		}
	}

	// Apply Dijkstra for each exit state and remember minimum distances
	while(!exit_states.empty()) {
		// Extract initial state
		CellPosition is = exit_states.front();
		exit_states.pop();

		// Initialize distances
		std::vector<std::vector<unsigned>> distances(height(), std::vector<unsigned>(width()));
		for (int row = 0; row < height(); row++) {
			for (int col = 0; col < width(); col++) {
				distances[row][col] = UINT_MAX;
			}
		}
		distances[is.first][is.second] = 0;

		// Vector of visited states
		std::vector<std::vector<bool>> visited(height(), std::vector<bool>(width()));
		for (int row = 0; row < height(); row++) {
			for (int col = 0; col < width(); col++) {
				visited[row][col] = false;
			}
		}

		// Queue of unprocessed successors
		std::queue<CellPosition> unprocessed;
		unprocessed.push(is);

		// Compute distances
		while(!unprocessed.empty()) {
			// Extract unprocessed
			CellPosition current = unprocessed.front();
			unprocessed.pop();
			visited[current.first][current.second] = true;
			int current_distance = distances[current.first][current.second];

			// Generate successors
			std::vector<CellPosition> successors = cell_neighbourhood(current);
			for(CellPosition successor: successors) {
				// Skip processed successors, update distance
				int r = successor.first, c = successor.second;
				if(!visited[r][c]) {
					visited[r][c] = true;
					if(current_distance + 1 < distances[r][c]) {
						distances[r][c] = current_distance + 1;
					}
					unprocessed.push(successor);
				}
			}
		}

		// Pick best distance
		for(int row = 0; row < height(); row++) {
			for(int col = 0; col < width(); col++) {
				Cell &c = cell(row, col);
				if(distances[row][col] < c.exit_distance) {
					c.exit_distance = distances[row][col];
				}
			}
		}
	}
}

CA CA::load(const std::string &filename) {
	// Load from image
   	CA ca = Bitmap::load(filename);

   	// Resolve distances
   	ca.recompute_shortest_paths();

	// Success
   	return ca;
}

void CA::show() {
    Bitmap::store(*this, "output.bmp");
}
