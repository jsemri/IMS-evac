/**
 * @file bitmap.h
 * Bitmap class implementation.
 * @author xandri03
 */

#include "bitmap.h" 
#include "evac_ca.h"

#include "bitmap_image.hpp" // bitmap library

// Predefined colors
constexpr rgb_t black = {0, 0, 0};
constexpr rgb_t grey = {128, 128, 128};
constexpr rgb_t white = {255, 255, 255};

constexpr rgb_t red = {255, 0, 0};
constexpr rgb_t green = {0, 255, 0};
constexpr rgb_t blue = {0, 0, 255};

constexpr rgb_t yellow = {255, 255, 0};

rgb_t Bitmap::translate(CellType type) {
    switch(type) {
        case Empty:
            return white;
        case Wall:
            return grey;
        case Person:
            return red;
        case Exit:
            return green;
        default:
            return black;
    }
}

CellType Bitmap::translate(rgb_t rgb) {
    if(rgb == white) {
        return Empty;
    } else if(rgb == grey) {
        return Wall;
    } else if(rgb == red) {
        return Person;
    } else if(rgb == green) {
        return Exit;
    } else {
        return Wall;
    }
}

void Bitmap::pick(image_drawer &pen, CellType type) {
    pen.pen_width(1);
    pen.pen_color(translate(type));
}

void Bitmap::pixel(image_drawer &pen, int row, int col) {
	pen.plot_pixel(row, col);
}

void Bitmap::hline(image_drawer &pen, int row, int colFrom, int colTo) {
	pen.horiztonal_line_segment(colFrom, colTo+1, row);
}

void Bitmap::vline(image_drawer &pen, int rowFrom, int rowTo, int col) {
	pen.vertical_line_segment(rowFrom, rowTo+1, col);
}

void Bitmap::rectangle(
	image_drawer &pen, int rowFrom, int colFrom, int rowTo, int colTo
) {
	for(int row = rowFrom; row <= rowTo; row++) {
		hline(pen, row, colFrom, colTo);
	}
}

EvacCA Bitmap::load(const std::string &filename) {
	// Open file
	bitmap_image image(filename);
	if(!image) {
		// Could not open file
		throw std::invalid_argument("could not open input file");
	}

	// Read dimensions
	int height = image.height();
    int width = image.width();
	EvacCA ca(height, width);
	
   // Differentiate colors
    for(int row = 0; row < height; row++) {
		for(int col = 0; col < width; col++) {
			rgb_t rgb;
			image.get_pixel(row, col, rgb);
            ca.get_cell(row, col).cell_type = translate(rgb);
		}
	}
	
    // TODO Dijkstra

    // Success
	return ca;
}

void Bitmap::store(EvacCA &ca, const std::string &filename, unsigned scale){
    // Construct image
    int height = ca.height();
    int width = ca.width();
    bitmap_image image(width*scale, height*scale);

    // Differentiate types
    image_drawer pen(image);
    for(int row = 0; row < height; row++) {
        for(int col = 0; col < width; col++) {
            // Draw (scaled) cell
            CellType type = ca.get_cell(row, col).cell_type;
            pick(pen, type);
            for(unsigned i = 0; i < scale; i++) {
				for(unsigned j = 0; j < scale; j++) {
					pixel(pen, row*scale+i, col*scale+j);
				}
			}
        }
    }
    
    // Output
    image.save_image(filename);
}

void Bitmap::sample_1(int length, const std::string &filename) {
    // Construct image
	int width = length, height = length;
	bitmap_image image(width, height);
	image_drawer pen(image);

    // White background
    image.set_all_channels(255, 255, 255);
	
	// Walls
	pick(pen, Wall);
	hline(pen, 0, 0, width-1);
	hline(pen, height-1, 0, width-1);
	vline(pen, 0, height-1, 0);
	vline(pen, 0, height-1, width-1);
	
	// Exit
	pick(pen, Exit);
	pixel(pen, width/2, 0);
	
	// Output
	image.save_image(filename);
}

void Bitmap::sample_2(int length, const std::string &filename) {
    // Construct image
    int width = length, height = length;
    bitmap_image image(width, height);
	image_drawer pen(image);
    
    // White background
    image.set_all_channels(255, 255, 255);

    // Walls
    pick(pen, Wall);
    hline(pen, 0, 0, width-1);
	hline(pen, height-1, 0, width-1);
	vline(pen, 0, height-1, 0);
	vline(pen, 0, height-1, width-1);
    
    // Exits
    pick(pen, Exit);
    pixel(pen, width/2, 0);
    pixel(pen, width/2, height-1);
    
    // Obstacle
    pick(pen, Wall);
    rectangle(pen, width/4, height/4, 3*width/4, 3*height/4);
    
    // Output
    image.save_image(filename);
}
