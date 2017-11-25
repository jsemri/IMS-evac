/**
 * @file bitmap.h
 * Bitmap class interface.
 */

#ifndef __bitmap_h
#define __bitmap_h

#include "evacuation.h"
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
    /** Output scaling. */
    static unsigned scale;

    /** Translate cell type to color; unknown types are translated to black. */
    static rgb_t translate(Evacuation::CellType type);

    /** Translate color to cell type; unknown colors are translated to Wall. */
    static Evacuation::CellType translate(rgb_t rgb);

    /** Pick cell type color and set a drawer. */
    static void pick(image_drawer &drawer, Evacuation::CellType type);

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
     * @return instance of Evacuation::CA class
     * @throw invalid_argument if failed to process input file
     * @note loaded model might be populated
     */
    static Evacuation::CA load(const std::string &filename);

    /**
     * Store model description to a bitmap.
     * @param ca model to store
     * @param filename name of output file
     * @param scale blow image up
     */
    static void store(
        Evacuation::CA &ca, const std::string &filename
    );

    /**
     * Store heat map of exit distances to "distances.bmp".
     * @param ca model to store
     */
    static void display_distances(Evacuation::CA &ca);
/*
    static void sample_1(int length, const std::string &filename);
    static void sample_2(int length, const std::string &filename);
    static void sample_3(int length, const std::string &filename);
    static void sample_4(int length, const std::string &filename);
*/
};

#endif
