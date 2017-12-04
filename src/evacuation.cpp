/**
 * @file evacuation.cpp
 * Cellular automaton definition.
 */

#pragma GCC diagnostic ignored "-Wsign-compare"
#pragma GCC diagnostic ignored "-Wimplicit-fallthrough="

#include <iostream>
#include <algorithm>
#include <ctime>
#include <cstdlib>
#include <cassert>

#include <climits>
#include <queue>
#include <sstream>

#include "evacuation.h"
#include "bitmap.h"

#define shuffle(arr) \
    std::random_shuffle(arr.begin(), arr.end())

#define RAND (((double) rand()) / (RAND_MAX))
#define PROB(val) val > RAND

using namespace Evacuation;

CA::CA(unsigned height, unsigned width) :
    height{height}, width{width},
    cells(height, std::vector<Cell>(width))
{}

std::vector<CellPosition> CA::cell_neighbourhood(
    size_t row, size_t col, int cell_types
) const {
    std::vector<CellPosition> neighbours;

    push_if(neighbours, row - 1, col, cell_types);
    push_if(neighbours, row, col - 1, cell_types);
    push_if(neighbours, row, col + 1, cell_types);
    push_if(neighbours, row + 1, col, cell_types);
    push_if(neighbours, row - 1, col - 1, cell_types);
    push_if(neighbours, row - 1, col + 1, cell_types);
    push_if(neighbours, row + 1, col - 1, cell_types);
    push_if(neighbours, row + 1, col + 1, cell_types);

    return neighbours;
}

std::vector<CellPosition> CA::cell_neighbourhood(
    CellPosition position, int cell_types
) const {
    return cell_neighbourhood(position.first, position.second, cell_types);
}

bool CA::evolve() {
    bool res = false;
    stat.time += 1;

    std::vector<CellPosition> people;
    std::vector<CellPosition> smoke_cells;
    for (size_t row = 0; row < this->height; row++) {
        for (size_t col = 0; col < this->width; col++) {
            auto &current = cells[row][col];
            switch (current.type) {
                case PersonAppearance:
                case Obstacle:
                case Empty:
                case Person:
                {
                    // propagation of smoke
                    float smoke_neigh =
                        cell_neighbourhood(row, col, SmokeCells).size();

                    float neigh =
                        cell_neighbourhood(row, col, ~(Exit | Wall)).size();
                    if (PROB( smoke_neigh/ neigh * smoke_spreading_rate)) {
                        smoke_cells.push_back(CellPosition(row, col));
                        /*
                        if (current.type == Obstacle) {
                            current.type = ObstacleWithSmoke;
                        }
                        else if (current.type == Person) {
                            current.type = PersonWithSmoke;
                            stat.smoke_exposed += 1;
                        }
                        else {
                            current.type = Smoke;
                        }*/
                    }
                    if (current.type == Person) {
                        // remember person position
                        people.push_back(CellPosition(row, col));
                    }
                    break;
                }
                case PersonAtExit:
                    // remove people at exits
                    stat.evac_time += stat.time;
                    if (stat.max_smoke_exposed < current.smoke_exposed) {
                        stat.max_smoke_exposed = current.smoke_exposed;
                    }
                    current.type = Exit;
                    //std::cout << "FIXME when -p 1" << std::endl;
                    break;
                case PersonWithSmoke:
                    stat.smoke_exposed += 1;
                    current.smoke_exposed += 1;
                    // remember person position
                    people.push_back(CellPosition(row, col));
                default:
                    ;
            }
        }
    }

    // Recompute exit distances
    recompute_shortest_paths();

    // Propagate smoke
    for (auto &c : smoke_cells) {
        auto & current = cell(c);
        if (current.type == Obstacle) {
            current.type = ObstacleWithSmoke;
        }
        else if (current.type == Person) {
            current.type = PersonWithSmoke;
            current.smoke_exposed++;
            stat.smoke_exposed += 1;
        }
        else {
            current.type = Smoke;
        }
    }

    // Propagate people
    res = !people.empty();
    shuffle(people);
    for (auto person : people) {
        auto neighbours = cell_neighbourhood(person);
        if (!neighbours.empty()) {
            // non-deterministic choice of the minimum
            shuffle(neighbours);
            // find min value
            CellPosition next_cell = neighbours[0];
            for (auto c : neighbours) {
                if (distance(next_cell) > distance(c)) {
                    next_cell = c;
                }
            }

            // distance to the next cell
            int person_distance = distance(person);
            int next_distance = distance(next_cell);
            int diff = person_distance - next_distance;
            // assert (diff == 0 || diff == -1 || diff == 1);
            // move to the cell with lesser exit distance or same distance
            // with some probability
            if (diff >= 1 ||
                (diff == 0 && PROB(chaos_rate)))
            {
                // move from empty or smoke cell
                stat.moves += 1;
                cell(person).type =
                    cell(person).type == Person ? Empty : Smoke;
                cell(next_cell).smoke_exposed = cell(person).smoke_exposed;
                cell(person).smoke_exposed = 0;
                auto &next_type = cell(next_cell).type;
                if (next_type == Smoke) {
                    next_type = PersonWithSmoke;
                }
                else if (next_type == Exit) {
                    next_type = PersonAtExit;
                }
                else {
                    next_type = Person;
                }
            }
        }
    }

    return res;
}

void CA::add_smoke(int smoke) {
    std::vector<CellPosition> empty_cells;
    for (size_t i = 0; i < this->height; i++) {
        for (size_t j = 0; j < this->width; j++) {
            if (cells[i][j].type == Empty) {
                empty_cells.push_back(CellPosition(i,j));
            }
        }
    }

    shuffle(empty_cells);
    if (smoke > 0 && smoke > empty_cells.size()) {
        throw std::logic_error("cannot have more smoke cells than empty cells");
    }

    while (!empty_cells.empty() && smoke-- > 0) {
        cell(empty_cells.back()).type = Smoke;
        empty_cells.pop_back();
    }

}

// somehow distribute people over empty cells
void CA::add_people(int people_count) {
    stat.pedestrians = people_count;
    std::vector<CellPosition> empty_cells;
    std::vector<CellPosition> empty_priority_cells;
    // mark all cells with possible person appearance
    for (size_t i = 0; i < this->height; i++) {
        for (size_t j = 0; j < this->width; j++) {
            if (cells[i][j].type == Empty) {
                empty_cells.push_back(CellPosition(i,j));
            }
            else if (cells[i][j].type == PersonAppearance) {
                empty_priority_cells.push_back(CellPosition(i,j));
            }
        }
    }

    if (people_count > empty_cells.size() + empty_priority_cells.size()) {
        throw std::logic_error("cannot have more people than empty cells");
    }

    shuffle(empty_priority_cells);
    shuffle(empty_cells);

    empty_cells.insert(
        empty_cells.end(), empty_priority_cells.begin(),
        empty_priority_cells.end());

    // placing people according to the priority
    while (!empty_cells.empty() && people_count-- > 0) {
        cell(empty_cells.back()).type = Person;
        empty_cells.pop_back();
    }
}

void CA::recompute_shortest_paths() {
    // Reset exit distances
    std::vector<std::vector<double>> distances(
		this->height, std::vector<double>(this->width)
	);

	// State distance accruals
    std::vector<std::vector<double>> accruals(
		this->height, std::vector<double>(this->width)
	);

	// Vector of visited states
    std::vector<std::vector<bool>> visited(
		this->height, std::vector<bool>(this->width)
	);

	// Vector of pushed states
    std::vector<std::vector<bool>> pushed(
		this->height, std::vector<bool>(this->width)
	);

	// Initialize
	for (unsigned row = 0; row < this->height; row++) {
        for (unsigned col = 0; col < this->width; col++) {
        	// Accrual
        	double accrual = 1.0;
        	CellType type = cell(row,col).type;
            if(type & (Person | PersonWithSmoke)) {
                accrual *= occupied_distance;
            }
            if(type & (Smoke | PersonWithSmoke)) {
                accrual *= smoke_distance;
            }
            accruals[row][col] = accrual;

            // Distance
            distances[row][col] = (double)UINT_MAX;

            // Visited and pushed
            visited[row][col] = false;
            pushed[row][col] = false;
        }
    }

    // Vector of unprocessed states
    std::vector<CellPosition> unprocessed;

    // Push exit states
    for(auto es: exit_states) {
    	unprocessed.push_back(es);
    	distances[es.first][es.second] = 0.0;
    	pushed[es.first][es.second] = true;
    }

    // Process all states
    while(!unprocessed.empty()) {
		// Find unprocessed state with minimum exit distance
		CellPosition current = unprocessed[0];
		int min = 0;
		double current_distance = distances[current.first][current.second];
		for(int i = 1; i < unprocessed.size(); i++) {
			CellPosition cp = unprocessed[i];
            double d = distances[cp.first][cp.second];
            if(d < current_distance) {
                min = i;
                current = cp;
                current_distance = d;
			}
		}

		// Visit minimum
        unprocessed[min] = unprocessed.back();
		unprocessed.pop_back();
		visited[current.first][current.second] = true;

        // Cell types that are considered reachable
        constexpr int succTypes =  Empty | Exit | Person | Smoke
           | PersonAppearance | PersonAtExit | PersonWithSmoke;

        // Generate successors
        std::vector<CellPosition> successors;
        successors = cell_neighbourhood(current, succTypes);

        // Compute successor distance
        double accrual = accruals[current.first][current.second];
        double next_distance = current_distance + accrual;

        // Process all successors
        for(CellPosition successor: successors) {
            // Skip processed successors
            unsigned r = successor.first, c = successor.second;
            if(!visited[r][c]) {
                if(next_distance < distances[r][c]) {
                    distances[r][c] = next_distance;
                    if(!pushed[r][c]) {
                    	unprocessed.push_back(successor);
                    	pushed[r][c] = true;
                    }
                }
            }
        }
    }

    // Store final result
    for(unsigned row = 0; row < this->height; row++) {
        for(unsigned col = 0; col < this->width; col++) {
        	cell(row,col).exit_distance = distances[row][col];
        }
    }
}

CA CA::load(const std::string &filename) {
    // Load from image
    CA ca = Bitmap::load(filename);

    // Identify exit states
    for(unsigned row = 0; row < ca.height; row++) {
        for(unsigned col = 0; col < ca.width; col++) {
        	if(ca.cell(row, col).type == Exit) {
                ca.exit_states.push_back(CellPosition(row,col));
            }
        }
    }

    // Resolve distances
    ca.recompute_shortest_paths();

    // Success
    return ca;
}

CA CA::copy() {
	CA cpy = CA(height, width);
	cpy.cells = cells;
	cpy.exit_states = exit_states;
	cpy.stat = stat;
	return cpy;
}

void CA::show() {
    Bitmap::store(*this, "output.bmp");
}


std::string Statistics::str() const noexcept {
	std::ostringstream ss;
    ss << "*********************************************************\n";
    float travelled = moves * cell_width;
    float realtime = time * time_step;
    ss << "Number of pedestrians              : " << pedestrians
        << std::endl;
    ss << "Total evacuation time              : " << realtime << " s"
        << std::endl;
    ss << "Mean time per person in smoke      : "
        << smoke_exposed * time_step / pedestrians << " s" << std::endl;
    ss << "Max time in smoke                  : "
        << max_smoke_exposed * time_step << " s" << std::endl;
    ss << "Mean evacuation time per person    : "
        << evac_time * time_step / pedestrians << " s" << std::endl;
    ss << "Total distance travelled           : " << travelled << " m"
        << std::endl;
    ss << "Mean distance travelled per person : "
        << travelled / pedestrians << " m" << std::endl;
    ss << "*********************************************************\n";

    return ss.str();
}

void Statistics::aggregate(Statistics &other) {
	time += other.time;
	smoke_exposed += other.smoke_exposed;
	moves += other.moves;
	evac_time += other.evac_time;
	max_smoke_exposed += other.max_smoke_exposed;
}

void Statistics::normalize(unsigned runs) {
	time /= runs;
	smoke_exposed /= runs;
	moves /= runs;
	evac_time /= runs;
    max_smoke_exposed /= runs;
}
