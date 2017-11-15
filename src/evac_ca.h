#ifndef __evac_ca_h
#define __evac_ca_h

#include <vector>
#include <iostream>

/** Type of a cell. */
enum CellType {
    Empty = 0,
    Exit,
    Wall,
    Person
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
	int exit_distance;
	
	union {
		// only for empty cell
		// the higher number the higher priority
		int person_occurence_priority;
		// only for person
		bool evacuated;
		// XXX maybe add more properties for other cell types
	};

	Cell() :
		type{Empty}, row{-1}, col{-1}, exit_distance{-1}
	{}
};

class EvacCA {
public:
    // XXX some additional parameters may be added later
    // considered parameters: chaos
    EvacCA(unsigned height, unsigned width);
    ~EvacCA() = default;

	///
    bool evolve();
    
    ///
    void add_people(int people);
	
	/**
	 * Load model description from a bitmap.
	 * @param filename name of input file
	 * @return instance of EvacCA class
	 * @throw invalid_argument if failed to process input file
	 * @note loaded model might be populated
	 */
    static EvacCA load(const std::string &filename);
    
    /// Store model description to "output.bmp".
	void show();
    
    // Inline methods:
    
    /// Dimension getter: number of rows
    inline int height() const {
        return cells.size();
    }
    
    /// Dimension getter: number of columns
    inline int width() const {
        return cells[0].size();
    }
	
	/// Retrieve a cell at a specified position
    inline Cell& cell(int row, int col) {
        return cells[row][col];
    }
    
private:
	/// Position in matrix. 
    using CellPosition = std::pair<size_t, size_t>;

    /// 2D matrix of cells.
    std::vector<std::vector<Cell>> cells;
    
    /// Positions of the people to evacuate.
    std::vector<CellPosition> people;

	///
    std::vector<CellPosition> cell_neighbourhood(CellPosition position) const;

	/// Recompute exit distances.
	void recompute_shortest_paths();
	
    // Inline methods:

    /// returns true if cell at specified position is empty or it is an exit
    inline bool is_empty(size_t row, size_t col) const {
        return cells[row][col].type == Empty ||
               cells[row][col].type == Exit; // XXX Exit?
    };

    /// push an position(row,col) to the vector if cell at this position is
 	/// empty
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
    
    /// Retrieve a cell at a specified position
    inline Cell& cell(CellPosition pos) {
		return cell(pos.first, pos.second);
    }
};

#endif
