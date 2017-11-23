/**
 * @file bitmap.h
 * Bitmap class implementation.
 * @author xandri03
 */

#include "bitmap.h"
#include "evacuation.h"

#include "bitmap_image.hpp" // bitmap library

using namespace Evacuation;

// Predefined colors
constexpr rgb_t black = {0, 0, 0};
constexpr rgb_t grey = {128, 128, 128};
constexpr rgb_t white = {255, 255, 255};

constexpr rgb_t red = {255, 0, 0};
constexpr rgb_t green = {0, 255, 0};
constexpr rgb_t blue = {0, 0, 255};

constexpr rgb_t yellow = {255, 255, 0};
constexpr rgb_t lightred = {255, 102, 102};
constexpr rgb_t brown = {51, 25, 0};

constexpr rgb_t lightpink = {255, 204, 204};
constexpr rgb_t lightgrey = {160, 160, 160};
constexpr rgb_t orange = {255, 128, 0};

rgb_t Bitmap::translate(CellType type) {
    switch(type) {
        case Empty:
        case PersonAppearance:
            return white;
        case Wall:
            return brown;
        case Person:
        case PersonAtExit:
            return red;
        case Exit:
            return green;
        case Smoke:
            return grey;
        case PersonWithSmoke:
            return lightred;
        case ObstacleWithSmoke:
            return lightgrey;
        case Obstacle:
            return orange;
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
    } else if (rgb == lightpink) {
        return PersonAppearance;
    } else if (rgb == orange) {
        return Obstacle;
    } else if (rgb == lightgrey) {
        return ObstacleWithSmoke;
    }
    else {
        return Wall;
    }
}

void Bitmap::pick(image_drawer &pen, CellType type) {
    pen.pen_width(1);
    pen.pen_color(translate(type));
}

void Bitmap::pixel(image_drawer &pen, int row, int col) {
    pen.plot_pixel(col, row);
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

CA Bitmap::load(const std::string &filename) {
    // Open file
    bitmap_image image(filename);
    if(!image) {
        // Could not open file
        throw std::invalid_argument("could not open input file");
    }

    // Read dimensions
    int height = image.height();
    int width = image.width();
    CA ca(height, width);

   // Differentiate colors
    for(int row = 0; row < height; row++) {
        for(int col = 0; col < width; col++) {
            rgb_t rgb;
            image.get_pixel(col, row, rgb);
            ca.cell(row,col).type = translate(rgb);
        }
    }

    // Success
    return ca;
}

void Bitmap::store(CA &ca, const std::string &filename, unsigned scale){
    // Construct image
    int height = ca.height();
    int width = ca.width();
    bitmap_image image(width*scale, height*scale);
    image_drawer pen(image);

    // Differentiate types
    for(int row = 0; row < height; row++) {
        for(int col = 0; col < width; col++) {
            // Draw (scaled) cell
            pick(pen, ca.cell(row, col).type);
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

void Bitmap::display_distances(CA &ca, unsigned scale) {
     // Construct image
    int height = ca.height();
    int width = ca.width();
    bitmap_image image(width*scale, height*scale);

    // Construct heat map
    for(int row = 0; row < height; row++) {
        for(int col = 0; col < width; col++) {
            unsigned distance = ca.cell(row,col).exit_distance;
            rgb_t color;
            if(distance >= 200) {
                color = black;
            } else {
                color = jet_colormap[999-distance*5];
            }
            for(unsigned i = 0; i < scale; i++) {
                for(unsigned j = 0; j < scale; j++) {
                    image.set_pixel(col*scale+j, row*scale+i, color);
                }
            }
            
        }
    }

    // Output
    image.save_image("distances.bmp");
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

void Bitmap::sample_3(int length, const std::string &filename) {
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
    pixel(pen, 0, 0);

    // Obstacles
    pick(pen, Wall);
    rectangle(pen, height/8, width/8, 5*height/8, 2*width/8);
    rectangle(pen, 6*height/8, 4*width/8, 7*height/8, 7*width/8);
    rectangle(pen, 2*height/8, 5*width/8, 4*height/8, 7*width/8);

    // Output
    image.save_image(filename);
}

void Bitmap::sample_4(int length, const std::string &filename) {
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
    pixel(pen, width/2-1, 0);
    pixel(pen, width/2+1, 0);

    // Output
    image.save_image(filename);
}
