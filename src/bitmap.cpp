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
            return black;
        case Obstacle:
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
    } else if(rgb == black) {
        return Wall;
    } else if(rgb == grey) {
        return Obstacle;
    } else if(rgb == red) {
        return Person;
    } else if(rgb == green) {
        return Exit;
    } else {
        return Wall;
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

void Bitmap::store(EvacCA &ca, const std::string &filename){
    // Construct image
    int height = ca.height();
    int width = ca.width();
    bitmap_image image(width, height);

    // Differentiate types
    image_drawer draw(image);
    draw.pen_width(1);
    for(int row = 0; row < height; row++) {
        for(int col = 0; col < width; col++) {
            CellType type = ca.get_cell(row, col).cell_type;
            draw.pen_color(translate(type));
            draw.plot_pixel(row, col);
        }
    }
    
    // Output
    image.save_image(filename);
}

void Bitmap::sample_1(int length, const std::string &filename) {
    // Construct image
	int width = length, height = length;
	bitmap_image image(width, height);

    // White background
    image.set_all_channels(255, 255, 255);

    // Set pen up
	image_drawer draw(image);
	draw.pen_width(1);
	
	// Walls
	draw.pen_color(black);
    draw.horiztonal_line_segment(0, width, 0);
	draw.horiztonal_line_segment(0, width, height-1);
	draw.vertical_line_segment(0, height, 0);
	draw.vertical_line_segment(0, height, height-1);
	
	// Exit
	draw.pen_color(green);
	draw.plot_pixel(width/2,0);
	
	// Output
	image.save_image(filename);
}

void Bitmap::sample_2(int length, int radius, const std::string &filename) {
    // Construct image
    int width = length, height = length;
    bitmap_image image(width, height);

    // White background
    image.set_all_channels(255, 255, 255);

    // Set pen up
    image_drawer draw(image);
    draw.pen_width(1);
    
    // Walls
    draw.pen_color(black);
    draw.horiztonal_line_segment(0, width, 0);
    draw.horiztonal_line_segment(0, width, height-1);
    draw.vertical_line_segment(0, height, 0);
    draw.vertical_line_segment(0, height, height-1);
    
    // Exits
    draw.pen_color(green);
    draw.plot_pixel(width/2,0);
    draw.plot_pixel(width/2,height-1);
    
    // Obstacle
    draw.pen_width(3);
    draw.pen_color(grey);
    draw.circle(width/2, height/2, radius);
    
    // Output
    image.save_image(filename);
}
