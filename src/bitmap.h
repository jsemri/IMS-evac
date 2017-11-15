/**
 * @file bitmap.h
 * Bitmap class interface.
 * @author xandri03
 */

#ifndef __bitmap_h
#define __bitmap_h

#include "evac_ca.h"
#include "bitmap_image.hpp"

/**
 * Model input/output via bitmaps.
 * @note For now all methods are static, but there is a possibility
 * that we will want to instantiate model writer for the sake of efficiency
 * (or just add initializers?)
 * @note separate classes for Pen/Image?
 */
class Bitmap {

private:

	/** Translate cell type to color; unknown types are translated to black. */
	static rgb_t translate(CellType type);

	/** Translate color to cell type; unknown colors are translated to Wall. */
	static CellType translate(rgb_t rgb);

	/** Pick cell type color and set a drawer. */
	static void pick(image_drawer &drawer, CellType type);
	
	/** Draw cell.*/
	static void pixel(image_drawer &pen, int row, int col);
	
	/** Draw horizontal line. */
	static void hline(image_drawer &pen, int row, int colFrom, int colTo);
	
	/** Draw vertical line. */
	static void vline(image_drawer &pen, int rowFrom, int rowTo, int col);
	
	/** Draw rectangle. */
	static void rectangle(
		image_drawer &pen, int rowFrom, int colFrom, int rowTo, int colTo
	);
public:

	/**
	 * Load model description from a bitmap.
	 * @param filename name of input file
	 * @return instance of EvacCA class
	 * @throw invalid_argument if failed to process input file
	 * @note loaded model might be populated
	 */
	static EvacCA load(const std::string &filename);

	/**
	 * Store model description to a bitmap.
	 * @param ca model to store
	 * @param filename name of output file
	 * @param scale blow image up
	 */
	static void store(
		EvacCA &ca, const std::string &filename, unsigned scale = 10
	);

	/**
	 * Display heat map of exit distances.
	 * @param ca model to store
	 */
	static void display_distances(EvacCA &ca);
	
	/**
 	 * Produce sample model: square room with one exit.
 	 * @param length room dimensions
 	 * @param filename name of output file
 	 */
	static void sample_1(int length, const std::string &filename);

	/**
 	 * Produce sample model: square room with one rectable obstacle and two
 	 * exits.
 	 * @param length room dimensions
 	 * @param filename name of output file
 	 */
	static void sample_2(int length, const std::string &filename);
};

#endif
