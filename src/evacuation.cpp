#pragma GCC diagnostic ignored "-Wsign-compare"
#pragma GCC diagnostic ignored "-Wimplicit-fallthrough="

#include <iostream>
#include <algorithm>
#include <ctime>
#include <cstdlib>
#include <cassert>

#include <climits>
#include <queue>

#include "evacuation.h"
#include "bitmap.h"

#define shuffle(arr) \
    std::random_shuffle(arr.begin(), arr.end())

#define RAND (((double) rand()) / (RAND_MAX))
#define PROB(val) val > RAND
#define POW2(x) x*x
#define POW3(x) x*x*x

#define SMOKE_SPREADING_RATE 0.2
#define FAINT_DEATH_RATE 0.04
#define DIRECTION_CHANGE_RATE 0.95
#define MOVE_NOT_CLOSER_COEF 0.25

using namespace Evacuation;

CA::CA(unsigned y, unsigned x) :
    cells(y, std::vector<Cell>(x)), pedestrians{0}, time{0}, casualties{0},
    moves{0}, max_distance{-1}
{}

std::vector<CellPosition>
CA::cell_neighbourhood(size_t row, size_t col) const {
    std::vector<CellPosition> neighbours;

    push_if_empty(neighbours, row - 1, col);
    push_if_empty(neighbours, row, col - 1);
    push_if_empty(neighbours, row, col + 1);
    push_if_empty(neighbours, row + 1, col);
    push_if_empty(neighbours, row - 1, col - 1);
    push_if_empty(neighbours, row - 1, col + 1);
    push_if_empty(neighbours, row + 1, col - 1);
    push_if_empty(neighbours, row + 1, col + 1);

    return neighbours;
}
std::vector<CellPosition>
CA::cell_neighbourhood2(size_t row, size_t col) const {
    std::vector<CellPosition> neighbours;

    push_if_empty2(neighbours, row - 1, col);
    push_if_empty2(neighbours, row, col - 1);
    push_if_empty2(neighbours, row, col + 1);
    push_if_empty2(neighbours, row + 1, col);
    push_if_empty2(neighbours, row - 1, col - 1);
    push_if_empty2(neighbours, row - 1, col + 1);
    push_if_empty2(neighbours, row + 1, col - 1);
    push_if_empty2(neighbours, row + 1, col + 1);

    return neighbours;
}

std::vector<CellPosition>
CA::cell_neighbourhood(CellPosition position) const {
    return cell_neighbourhood(position.first, position.second);
}

bool CA::evolve() {
    bool res = false;
    time++;

    std::vector<CellPosition> people;
    for (size_t row = 0; row < height(); row++) {
        for (size_t col = 0; col < width(); col++) {
            auto &current = cells[row][col];
            switch (current.type) {
                case PersonAppearance:
                case Obstacle:
                case Empty:
                {
                    // propagation of smoke
                    auto neighbours =
                        cell_neighbourhood2(row, col);
                    int smoke_prob = 0;
                    for (auto i : neighbours) {
                        smoke_prob +=
                            cell(i).type == Smoke ||
                            cell(i).type == SmokeWithPerson ||
                            cell(i).type == SmokeWithObstacle;
                    }
                    if (PROB(smoke_prob / 8.0 * SMOKE_SPREADING_RATE)) {
                        if (current.type == Obstacle)
                            current.type = SmokeWithObstacle;
                        else
                            current.type = Smoke;
                    }
                    break;
                }
                case PersonAtExit:
                    // remove people at exits
                    current.type = Exit;
                    break;
                case SmokeWithPerson:
                    // with 0.1 probability person in smoke faints and dies
                    if (PROB(FAINT_DEATH_RATE)) {
                        current.type = Smoke;
                        casualties++;
                        break;
                    }
                case Person:
                    // remember person position
                    people.push_back(CellPosition(row, col));
                default:
                    ;
            }
        }
    }

    res = !people.empty();
    shuffle(people);
    for (auto person : people) {
        auto neighbours = cell_neighbourhood(person);
        if (!neighbours.empty()) {
            // find min value
            CellPosition next_cell = neighbours[0];
            for (auto c : neighbours) {
                if (distance(next_cell) > distance(c)) {
                    next_cell = c;
                }
            }

            // non-deterministic choice of the minimum
            for (size_t j = 0; j < neighbours.size(); j++) {
                if (distance(next_cell) == distance(neighbours[j])) {
                    next_cell = neighbours[j];
                    if (cell(next_cell).type == Smoke) {
                        continue;
                    }
                    if (PROB(DIRECTION_CHANGE_RATE)) {
                        break;
                    }
                }
            }

            // distance to the next cell
            int person_distance = distance(person);
            int next_distance = distance(next_cell);
            int diff = person_distance - next_distance;
            assert (diff == 0 || diff == -1 || diff == 1);
            // move to the cell with lesser exit distance or same distance
            // with some probability
            if (diff == 1 ||
//                (diff == 0 && PROB(POW2(1.0 * person_distance / max_distance))))
                (diff == 0 && PROB(MOVE_NOT_CLOSER_COEF)))
            {
                // move from empty or smoke cell
                moves++;
                cell(person).type =
                    cell(person).type == Person ? Empty : Smoke;
                auto &next_type = cell(next_cell).type;
                if (next_type == Smoke) {
                    next_type = SmokeWithPerson;
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
    for (size_t i = 0; i < height(); i++) {
        for (size_t j = 0; j < width(); j++) {
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
    pedestrians = people_count;
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
        std::vector<std::vector<unsigned>> distances(
			height(), std::vector<unsigned>(width())
		);
        for (int row = 0; row < height(); row++) {
            for (int col = 0; col < width(); col++) {
                distances[row][col] = UINT_MAX;
            }
        }
        distances[is.first][is.second] = 0;

        // Vector of visited states
        std::vector<std::vector<bool>> visited(
			height(), std::vector<bool>(width())
		);
        for (int row = 0; row < height(); row++) {
            for (int col = 0; col < width(); col++) {
                visited[row][col] = false;
            }
        }

        // Queue of unprocessed successors
        std::vector<CellPosition> unprocessed;
        unprocessed.push_back(is);

        // Compute distances
        while(!unprocessed.empty()) {
			// Find unprocessed state with minimum exit distance
			int min = 0;
			CellPosition *cp_min = &unprocessed[min];
			for(int i = 1; i < unprocessed.size(); i++) {
				CellPosition cp = unprocessed[i];
				if(
					distances[cp.first][cp.second] <
					distances[cp_min->first][cp_min->second]
				) {
					min = i;
					cp_min = &unprocessed[min];
				}
			}

			// Extract minimum
            CellPosition current = unprocessed[min];
			unprocessed[min] = unprocessed.back();
			unprocessed.pop_back();
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
                    unprocessed.push_back(successor);
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

void CA::compute_max_distance() {
    for(int row = 0; row < height(); row++) {
        for(int col = 0; col < width(); col++) {
            if (cells[row][col].exit_distance > max_distance) {
                max_distance = cells[row][col].exit_distance;
            }
        }
    }
}

CA CA::load(const std::string &filename) {
    // Load from image
    CA ca = Bitmap::load(filename);

    // Resolve distances
    ca.recompute_shortest_paths();
    // compute max distance
    ca.compute_max_distance();

    // Success
    return ca;
}

void CA::show() {
    Bitmap::store(*this, "output.bmp");
}

void CA::print_statistics() const noexcept {
    // TODO exits - number, size, loading
    std::cout << "*********************************************************\n";
    float traveled = 0.4 * moves;
    float realtime = time * 0.4;
    int evacuated = pedestrians - casualties;
    std::cout << "Total pedestrians                 : " << pedestrians
        << std::endl;
    std::cout << "People evacuated                  : " << evacuated
        << std::endl;
    std::cout << "Casualties                        : " << casualties
        << std::endl;
    std::cout << "Casualties %                      : "
        << casualties * 1.0 / pedestrians  << std::endl;
//    std::cout << "Exit total size                   : " << std::endl;
    std::cout << "Total evacuation time             : " << realtime << " s"
        << std::endl;
    std::cout << "Mean evacuation time per person   : "
        << realtime / evacuated << " s" << std::endl;
    std::cout << "Total distance traveled           : " << traveled << " m"
        << std::endl;
    std::cout << "Mean distance traveled per person : "
        << traveled / pedestrians << " m" << std::endl;
    std::cout << "*********************************************************\n";
}
