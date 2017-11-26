/**
 * @file main.cpp
 * Simulation control.
 */

#include <iostream>
#include <ctime>

#include <unistd.h>
#include <getopt.h>
#include <sys/types.h>

#include "evacuation.h"
#include "bitmap.h"    // remove after debug

/** --help string. */
static const char *helpstr =
"Program for simulating evacuation of building.\n"
"Usage: evac INPUT [OPTIONS] ...\n"
"  -h            : show this help and exit\n"
"  -t <DELAY>    : set delay of next step of evolution in ms, default 300\n"
"  -p <N>        : number of people to evacuate, default 100\n"
"  -s <N>        : number of cells with smoke, default 0\n";

/** Entry point. */
int main(int argc, char **argv) {
    // Arguments:
    long delay = 1000 * 300;    // simulation delay (microseconds)
    int people = 100;   // persons to evacuate
    int smoke = 0;     // cells with smoke
    
    // Process program arguments
    int c;              // reading the options
    int opt_cnt = 1;    // used for locating positional argument
    while ((c = getopt(argc, argv, "ht:p:s:")) != -1) {
        opt_cnt += 2;
        switch (c) {
            case 'h':
                fprintf(stderr, "%s", helpstr);
                return EXIT_SUCCESS;
            case 't':
                delay = 1000 * std::stoi(optarg); // convert to microseconds
                break;
            case 'p':
                people = std::stoi(optarg);
                break;
            case 's':
                smoke = std::stoi(optarg);
                break;
            default:
                return EXIT_FAILURE;
        }
    }
    // Check positional argument
    if (argc - opt_cnt != 1) {
        std::cerr << "Error: invalid arguments\n";
        return EXIT_FAILURE;
    }
    char *filename = argv[opt_cnt];

    // Load the model
    try {
        // Seed
        std::srand(std::time(0));
        
        // Load model from a bitmap
        Evacuation::CA model = Evacuation::CA::load(filename);
    
        // Populate the model and add smoke
        model.add_people(people);
        model.add_smoke(smoke);
        
        // Uncoment this to open image with xdg-open
        if (delay > 0) {
            // Display exit distances
            Bitmap::display_distances(model);

            // Display the model
            model.show();

            std::system("xdg-open output.bmp");
        }        

        // Simulate n times and display aggregate statistics
        Evacuation::Statistics stat;
        stat.pedestrians = people;
        for(int i = 0; i < 2; i++) {
            // Copy the CA
            Evacuation::CA ca = model.copy();

            // Evolve CA in loop until CA can't change its states
            while (ca.evolve()) {
                if (delay > 0) {
                    // Show the current state of CA
                    ca.show();
                    usleep(delay);
                }
            }

            // Show the final state of CA
            ca.show();  // comment this for statistic collecting

            // Collect statistics
            std::cout << ca.stat.str();
            stat.aggregate(ca.stat);
            stat.runs++;
        }

        // Normalize and display statistics
        stat.normalize();
        std::cout << stat.str();
    }
    catch (std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        exit(EXIT_FAILURE);
    }

    // Success
    return EXIT_SUCCESS;
}

